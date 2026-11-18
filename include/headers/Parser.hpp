
#pragma once

#include <vector>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <numbers>
#include <utility>

enum class TokenType {
    Number,
    Variable,
    Operator,
    Function,
    LeftParen,
    RightParen
};

struct Token
{
    TokenType type;
    double value = 0.0;
    std::string text;
    int precedence = 0;
    bool rightAssociative = false;
};

// Checks whether the previous token means a unary operator can appear.
bool isUnaryPosition(const std::vector<Token>& tokens)
{
    if (tokens.empty())
        return true;

    TokenType previous = tokens.back().type;

    return previous == TokenType::LeftParen ||
        previous == TokenType::Operator;
}

// Tokenizer
std::vector<Token> tokenize(const std::string& expr)
{
    std::vector<Token> tokens;

    for (size_t i = 0; i < expr.length(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(expr[i]);

        if (std::isspace(c))
            continue;

        // Numbers
        if (std::isdigit(c) || expr[i] == '.')
        {
            size_t start = i;

            bool hasDigit = false;
            bool hasDecimalPoint = false;

            while (i < expr.length())
            {
                char current = expr[i];

                if (std::isdigit(static_cast<unsigned char>(current)))
                {
                    hasDigit = true;
                    ++i;
                }
                else if (current == '.' && !hasDecimalPoint)
                {
                    hasDecimalPoint = true;
                    ++i;
                }
                else
                {
                    break;
                }
            }

            if (!hasDigit)
                throw std::runtime_error("Invalid number");

            std::string num = expr.substr(start, i - start);

            tokens.push_back({
                TokenType::Number,
                std::stod(num),
                num
                });

            --i;
        }

        // Identifiers: x, pi, e, sin, cos, etc.
        else if (std::isalpha(c))
        {
            size_t start = i;

            while (i < expr.length() &&
                std::isalpha(static_cast<unsigned char>(expr[i])))
            {
                ++i;
            }

            std::string id = expr.substr(start, i - start);
            --i;

            if (id == "x")
            {
                tokens.push_back({
                    TokenType::Variable, 0.0, id
                    });
            }
            else if (id == "pi")
            {
                tokens.push_back({
                    TokenType::Number, std::numbers::pi, id
                    });
            }
            else if (id == "e")
            {
                tokens.push_back({
                    TokenType::Number, std::numbers::e, id
                    });
            }
            else
            {
                tokens.push_back({
                    TokenType::Function, 0.0, id
                    });
            }
        }

        // Parentheses
        else if (expr[i] == '(')
        {
            tokens.push_back({
                TokenType::LeftParen, 0.0, "("
                });
        }
        else if (expr[i] == ')')
        {
            tokens.push_back({
                TokenType::RightParen, 0.0, ")"
                });
        }

        // Operators
        else
        {
            std::string op(1, expr[i]);

            if (op != "+" && op != "-" &&
                op != "*" && op != "/" && op != "^")
            {
                throw std::runtime_error("Unknown character: " + op);
            }

            // Unary plus and minus
            if ((op == "+" || op == "-") && isUnaryPosition(tokens))
            {
                tokens.push_back({
                    TokenType::Operator,
                    0.0,
                    op == "-" ? "neg" : "pos",
                    3,
                    true
                    });
            }
            else
            {
                int precedence =
                    (op == "+" || op == "-") ? 1 :
                    (op == "*" || op == "/") ? 2 : 4;

                bool rightAssociative = (op == "^");

                tokens.push_back({
                    TokenType::Operator,
                    0.0,
                    op,
                    precedence,
                    rightAssociative
                    });
            }
        }
    }

    return tokens;
}

// Implicit multiplication helpers
bool endsValue(TokenType type)
{
    return type == TokenType::Number || type == TokenType::Variable || type == TokenType::RightParen;
}

bool startsValue(TokenType type)
{
    return type == TokenType::Number || type == TokenType::Variable || type == TokenType::LeftParen || type == TokenType::Function;
}

// Insert multiplication where necessary: 2x, 3(x+1), etc.
std::vector<Token> insertImplicitMultiplication(
    const std::vector<Token>& tokens)
{
    std::vector<Token> result;

    for (const Token& token : tokens)
    {
        if (!result.empty() &&
            endsValue(result.back().type) &&
            startsValue(token.type))
        {
            result.push_back({
                TokenType::Operator, 0.0, "*", 2, false
                });
        }

        result.push_back(token);
    }

    return result;
}

// Shunting-yard: convert tokens to RPN
std::vector<Token> compileToRPN(const std::string& expr)
{
    auto tokens = insertImplicitMultiplication(tokenize(expr));

    std::vector<Token> output;
    std::stack<Token> opStack;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Number ||
            tok.type == TokenType::Variable)
        {
            output.push_back(tok);
        }
        else if (tok.type == TokenType::Function ||
            tok.type == TokenType::LeftParen)
        {
            opStack.push(tok);
        }
        else if (tok.type == TokenType::RightParen)
        {
            while (!opStack.empty() &&
                opStack.top().type != TokenType::LeftParen)
            {
                output.push_back(opStack.top());
                opStack.pop();
            }

            if (opStack.empty())
                throw std::runtime_error("Mismatched parentheses");

            opStack.pop();

            if (!opStack.empty() &&
                opStack.top().type == TokenType::Function)
            {
                output.push_back(opStack.top());
                opStack.pop();
            }
        }
        else if (tok.type == TokenType::Operator)
        {
            // Prefix unary operators must not pop existing operators.
            if (tok.text == "neg" || tok.text == "pos")
            {
                opStack.push(tok);
                continue;
            }

            while (!opStack.empty() &&
                opStack.top().type == TokenType::Operator &&
                ((!tok.rightAssociative &&
                    tok.precedence <= opStack.top().precedence) ||
                    (tok.rightAssociative &&
                        tok.precedence < opStack.top().precedence)))
            {
                output.push_back(opStack.top());
                opStack.pop();
            }

            opStack.push(tok);
        }
    }

    while (!opStack.empty())
    {
        if (opStack.top().type == TokenType::LeftParen ||
            opStack.top().type == TokenType::RightParen)
        {
            throw std::runtime_error("Mismatched parentheses");
        }

        output.push_back(opStack.top());
        opStack.pop();
    }

    return output;
}


inline double evaluateRPN(const std::vector<Token>& rpn, double x)
{
    std::vector<double> values;
    values.reserve(rpn.size());

    for (const Token& token : rpn)
    {
        if (token.type == TokenType::Number)
        {
            values.push_back(token.value);
        }
        else if (token.type == TokenType::Variable)
        {
            values.push_back(x);
        }
        else if (token.type == TokenType::Operator)
        {
            if (token.text == "neg" || token.text == "pos")
            {
                if (values.empty())
                    throw std::runtime_error("Missing operand");

                double a = values.back();
                values.pop_back();

                values.push_back(token.text == "neg" ? -a : a);
            }
            else
            {
                if (values.size() < 2)
                    throw std::runtime_error("Missing operand");

                double b = values.back();
                values.pop_back();

                double a = values.back();
                values.pop_back();

                if (token.text == "+") values.push_back(a + b);
                else if (token.text == "-") values.push_back(a - b);
                else if (token.text == "*") values.push_back(a * b);
                else if (token.text == "/") values.push_back(a / b);
                else if (token.text == "^") values.push_back(std::pow(a, b));
                else throw std::runtime_error("Unknown operator");
            }
        }
        else if (token.type == TokenType::Function)
        {
            if (values.empty())
                throw std::runtime_error("Missing function argument");

            double a = values.back();
            values.pop_back();

            if (token.text == "sin") values.push_back(std::sin(a));
            else if (token.text == "cos") values.push_back(std::cos(a));
            else if (token.text == "tan") values.push_back(std::tan(a));
            else if (token.text == "sqrt") values.push_back(std::sqrt(a));
            else if (token.text == "log") values.push_back(std::log(a));
            else if (token.text == "abs") values.push_back(std::abs(a));
            else throw std::runtime_error("Unknown function: " + token.text);
        }
    }

    if (values.size() != 1)
        throw std::runtime_error("Invalid expression");

    return values.back();
}

class Expression
{
public:
    explicit Expression(const std::string& expression)
        : m_rpn(compileToRPN(expression))
    {}

    double evaluate(double x) const
    {
        return evaluateRPN(m_rpn, x);
    }

private:
    std::vector<Token> m_rpn;
};
