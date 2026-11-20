#pragma once

#include <vector>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <numbers>
#include <utility>

// Defines the different types of elements that can appear
// in a mathematical expression.
enum class TokenType {
    Number,
    Variable,
    Operator,
    Function,
    LeftParen,
    RightParen
};

// Represents a single element extracted from the expression.
// Additional information is stored to help with operator processing.
struct Token
{
    TokenType type;
    double value = 0.0;
    std::string text;
    int precedence = 0;
    bool rightAssociative = false;
};

// Determines whether the next '+' or '-' should be interpreted
// as a unary operator rather than a binary one.
//
// A unary operator is expected at the beginning of an expression,
// after an opening parenthesis, or after another operator.
bool isUnaryPosition(const std::vector<Token>& tokens)
{
    if (tokens.empty())
        return true;

    TokenType previous = tokens.back().type;

    return previous == TokenType::LeftParen || previous == TokenType::Operator;
}

// TOKENIZATION
// Splits the input expression into individual tokens.
//
// For example, "2*x + sin(x)" becomes a sequence containing
// numbers, operators, functions, and parentheses.
std::vector<Token> tokenize(const std::string& expr)
{
    std::vector<Token> tokens;

    for (size_t i = 0; i < expr.length(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(expr[i]);

        // Ignore whitespace since it has no mathematical meaning.
        if (std::isspace(c))
            continue;

        // NUMBERS
        // Recognizes integer and decimal values.
        if (std::isdigit(c) || expr[i] == '.')
        {
            size_t start = i;

            bool hasDigit = false;
            bool hasDecimalPoint = false;

            // Read consecutive digits and allow at most one decimal point.
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

            // A decimal point by itself is not a valid number.
            if (!hasDigit)
                throw std::runtime_error("Invalid number");

            // Extract the original text and convert it to a double.
            std::string num = expr.substr(start, i - start);

            tokens.push_back({ TokenType::Number,std::stod(num),num });

            // The loop will increment i again, so move it back
            // to the last character belonging to this number.
            --i;
        }

        // IDENTIFIERS
        // Reads names such as x, pi, e, sin, cos, and sqrt.
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

            // The independent variable used when evaluating the function.
            if (id == "x")
            {
                tokens.push_back({ TokenType::Variable, 0.0, id });
            }
            // Mathematical constant pi.
            else if (id == "pi")
            {
                tokens.push_back({ TokenType::Number, std::numbers::pi, id });
            }
            // Mathematical constant e.
            else if (id == "e")
            {
                tokens.push_back({ TokenType::Number, std::numbers::e, id });
            }
            // Any other identifier is treated as a mathematical function.
            else
            {
                tokens.push_back({ TokenType::Function, 0.0, id });
            }
        }

        // PARENTHESES
        // Parentheses are used to group operations and control their order.
        else if (expr[i] == '(')
        {
            tokens.push_back({ TokenType::LeftParen, 0.0, "(" });
        }
        else if (expr[i] == ')')
        {
            tokens.push_back({ TokenType::RightParen, 0.0, ")" });
        }

        // OPERATORS
        // Recognizes the supported arithmetic operations.
        else
        {
            std::string op(1, expr[i]);

            if (op != "+" && op != "-" &&
                op != "*" && op != "/" && op != "^")
            {
                throw std::runtime_error("Unknown character: " + op);
            }

            // UNARY PLUS AND MINUS
            // Distinguishes signs such as "-x" from binary subtraction
            // such as "x-2".
            if ((op == "+" || op == "-") && isUnaryPosition(tokens))
            {
                tokens.push_back({ TokenType::Operator,0.0,op == "-" ? "neg" : "pos",3,true });
            }
            else
            {
                // Assigns precedence levels to binary operators.
                // Higher values represent operations with higher priority.
                int precedence =
                    (op == "+" || op == "-") ? 1 :
                    (op == "*" || op == "/") ? 2 : 4;

                // Exponentiation is right-associative:
                // a^b^c is interpreted as a^(b^c).
                bool rightAssociative = (op == "^");

                tokens.push_back({ TokenType::Operator,0.0,op,precedence,rightAssociative });
            }
        }
    }

    return tokens;
}

// IMPLICIT MULTIPLICATION HELPERS

// Checks whether a token can represent the end of a value.
// This is used to detect expressions such as "2x" or "(x+1)2".
bool endsValue(TokenType type)
{
    return type == TokenType::Number || type == TokenType::Variable || type == TokenType::RightParen;
}

// Checks whether a token can represent the beginning of a value.
// Functions and opening parentheses are included.
bool startsValue(TokenType type)
{
    return type == TokenType::Number || type == TokenType::Variable || type == TokenType::LeftParen || type == TokenType::Function;
}

// INSERT IMPLICIT MULTIPLICATION
// Adds multiplication operators where they are implied by the notation.
//
// Examples:
// "2x"       becomes "2*x"
// "3(x+1)"   becomes "3*(x+1)"
// "(x+1)(x-1)" becomes "(x+1)*(x-1)"
std::vector<Token> insertImplicitMultiplication(const std::vector<Token>& tokens)
{
    std::vector<Token> result;

    for (const Token& token : tokens)
    {
        // If the previous token ends a value and the current token
        // starts another one, multiplication must be inserted.
        if (!result.empty() &&
            endsValue(result.back().type) &&
            startsValue(token.type))
        {
            result.push_back({ TokenType::Operator, 0.0, "*", 2, false });
        }

        result.push_back(token);
    }

    return result;
}

// SHUNTING-YARD ALGORITHM
// Converts the token sequence into Reverse Polish Notation (RPN).
//
// RPN places operators after their operands, removing the need
// for parentheses during evaluation.
//
// For example:
// "2 + 3 * 4" becomes "2 3 4 * +".
std::vector<Token> compileToRPN(const std::string& expr)
{
    // Tokenize the expression and insert any missing multiplication signs.
    auto tokens = insertImplicitMultiplication(tokenize(expr));

    std::vector<Token> output;

    // Temporarily stores operators, functions, and parentheses
    // while the output sequence is being constructed.
    std::stack<Token> opStack;

    for (const auto& tok : tokens)
    {
        // Numbers and variables can be added directly to the output.
        if (tok.type == TokenType::Number ||
            tok.type == TokenType::Variable)
        {
            output.push_back(tok);
        }

        // Functions and opening parentheses are pushed onto the stack.
        else if (tok.type == TokenType::Function ||
            tok.type == TokenType::LeftParen)
        {
            opStack.push(tok);
        }

        // A closing parenthesis means that the current group is complete.
        else if (tok.type == TokenType::RightParen)
        {
            // Move operators to the output until the matching
            // opening parenthesis is found.
            while (!opStack.empty() &&
                opStack.top().type != TokenType::LeftParen)
            {
                output.push_back(opStack.top());
                opStack.pop();
            }

            // If no opening parenthesis exists, the expression is invalid.
            if (opStack.empty())
                throw std::runtime_error("Mismatched parentheses");

            // Remove the matching opening parenthesis.
            opStack.pop();

            // If a function is waiting immediately before the parenthesis,
            // place it after its argument in the RPN output.
            if (!opStack.empty() &&
                opStack.top().type == TokenType::Function)
            {
                output.push_back(opStack.top());
                opStack.pop();
            }
        }
        else if (tok.type == TokenType::Operator)
        {
            // Prefix unary operators are pushed directly onto the stack.
            // They must not remove existing operators.
            if (tok.text == "neg" || tok.text == "pos")
            {
                opStack.push(tok);
                continue;
            }

            // Compare operator precedence and associativity.
            // Higher-priority operators are moved to the output first.
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

            // Store the current operator for later processing.
            opStack.push(tok);
        }
    }

    // Move any remaining operators from the stack to the output.
    while (!opStack.empty())
    {
        // Any remaining parenthesis indicates an invalid expression.
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

// RPN EVALUATION
// Evaluates an expression already converted into Reverse Polish Notation.
//
// The supplied x value replaces every occurrence of the variable x.
// A stack is used to store intermediate calculation results.
inline double evaluateRPN(const std::vector<Token>& rpn, double x)
{
    std::vector<double> values;
    values.reserve(rpn.size());

    for (const Token& token : rpn)
    {
        // Numbers are pushed directly onto the evaluation stack.
        if (token.type == TokenType::Number)
        {
            values.push_back(token.value);
        }
        // Replace the variable with the x value supplied by the caller.
        else if (token.type == TokenType::Variable)
        {
            values.push_back(x);
        }
        else if (token.type == TokenType::Operator)
        {
            // UNARY OPERATORS
            // These operators require only one operand.
            if (token.text == "neg" || token.text == "pos")
            {
                if (values.empty())
                    throw std::runtime_error("Missing operand");

                double a = values.back();
                values.pop_back();

                // Apply the sign operation and store the result.
                values.push_back(token.text == "neg" ? -a : a);
            }
            else
            {
                // BINARY OPERATORS
                // These operations require two operands.
                if (values.size() < 2)
                    throw std::runtime_error("Missing operand");

                // The second operand is retrieved first because
                // it is at the top of the stack.
                double b = values.back();
                values.pop_back();

                double a = values.back();
                values.pop_back();

                // Perform the requested operation and push its result.
                if (token.text == "+") values.push_back(a + b);
                else if (token.text == "-") values.push_back(a - b);
                else if (token.text == "*") values.push_back(a * b);
                else if (token.text == "/") values.push_back(a / b);
                else if (token.text == "^") values.push_back(std::pow(a, b));
                else throw std::runtime_error("Unknown operator");
            }
        }
        // MATHEMATICAL FUNCTIONS
        // Functions take one argument, which is retrieved from the stack.
        else if (token.type == TokenType::Function)
        {
            if (values.empty())
                throw std::runtime_error("Missing function argument");

            double a = values.back();
            values.pop_back();

            // Apply the corresponding standard mathematical function.
            if (token.text == "sin") values.push_back(std::sin(a));
            else if (token.text == "cos") values.push_back(std::cos(a));
            else if (token.text == "tan") values.push_back(std::tan(a));
            else if (token.text == "sqrt") values.push_back(std::sqrt(a));
            else if (token.text == "log") values.push_back(std::log(a));
            else if (token.text == "abs") values.push_back(std::abs(a));
            else throw std::runtime_error("Unknown function: " + token.text);
        }
    }

    // A valid expression must leave exactly one result on the stack.
    // Any other number of values indicates an invalid expression.
    if (values.size() != 1)
        throw std::runtime_error("Invalid expression");

    return values.back();
}

// EXPRESSION CLASS
// Provides a simple interface for compiling and evaluating expressions.
//
// The expression is converted to RPN only once in the constructor.
// Subsequent evaluations reuse the compiled representation, which is
// useful when plotting a function for many different x values.
class Expression
{
public:
    // Compile the supplied mathematical expression into RPN.
    explicit Expression(const std::string& expression)
        : m_rpn(compileToRPN(expression))
    {}

    // Evaluate the compiled expression for a given x coordinate.
    double evaluate(double x) const
    {
        return evaluateRPN(m_rpn, x);
    }

private:
    // Stores the compiled expression so it does not need to be
    // tokenized and converted again for every evaluation.
    std::vector<Token> m_rpn;
};