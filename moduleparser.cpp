#include "moduleparser.h"

#include <stdexcept>

#include <QRegExp>

Parsed parsed = { QList<Variable>(), QList<Constant>(), QList<Block>() };

ModuleParser::ModuleParser()
{

}

void skipSpaces(const QString& input, int& i, int& line)
{
    while (i < input.length() && input[i].isSpace())
        if (input[i++] == '\n')
            line++;
}

bool isKeyword(const QString& word)
{
    QVector<QString> keywords = { "Variables", "Constants", "Blocks" };

    return keywords.indexOf(word) > -1;
}

const QString parseCommandOrDescriptionOrName(const QString& buf, int& i, int& line)
{
    i++;
    skipSpaces(buf, i, line);

    if (buf[i] != '"')
        throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                 .arg(buf[i]).arg(line).toStdString());

    QString result;

    i++;
    while (i < buf.length() && buf[i] != '"')
    {
        result += buf[i];
        if (buf[i++] == '\n')
            line++;
    }

    if (i >= buf.length() || buf[i] != '"')
        throw std::runtime_error(QString("Syntax error: '\"' is not found, line %1.").arg(line).toStdString());

    i++;
    skipSpaces(buf, i, line);

    return result;
}

const QString parseValue(const QString& buf, int& i, int& line)
{
    i++;
    skipSpaces(buf, i, line);

    QString value;

    if (buf[i] == '"')
    {
        i++;
        while (i < buf.length() && buf[i] != '"')
        {
            value += buf[i];
            if (buf[i++] == '\n')
                line++;
        }

        if (i >= buf.length() || buf[i] != '"')
            throw std::runtime_error(QString("Syntax error: '\"' is not found, line %1.").arg(line).toStdString());

        i++;
    }
    else if (buf[i].isNumber() || buf[i] == '-')
    {
        value += buf[i++];

        while (i < buf.length() && buf[i].isNumber())
            value += buf[i++];

        skipSpaces(buf, i, line);

        if (buf[i] == '.' || buf[i] == ',')
        {
            value += '.';
            i++;
            while (i < buf.length() && buf[i].isNumber())
                value += buf[i++];
        }

        if (value[value.length() - 1] == '.')
            value = value.remove(value.length() - 1, 1);

        skipSpaces(buf, i, line);
    }

    return value;
}

void parseVariables(const QString& block, int& line)
{
    QString buf = block.mid(block.indexOf('{') + 1, block.indexOf('}') - block.indexOf('{') - 1);

    for (int i = 0; i < buf.length(); i++)
    {
        if (buf[i].isSpace())
        {
            if (buf[i] == '\n')
                line++;
        }
        else if (buf[i].isLetter() || buf[i] == '_' || buf[i] == '!')
        {
            bool isNotEntered = false;

            if (buf[i] == '!')
            {
                isNotEntered = true;
                i++;
            }

            QString varName(buf[i++]);
            QString varDescription;
            QString varValue;

            while (i < buf.length() && (buf[i].isLetterOrNumber() || buf[i] == '_'))
                varName += buf[i++];

            skipSpaces(buf, i, line);

            if (buf[i] == ':')
                varDescription = parseCommandOrDescriptionOrName(buf, i, line);

            if (buf[i] == '=')
                varValue = parseValue(buf, i, line);

            if (buf[i] == ';')
                parsed.variables.push_back({ varName, varDescription, varValue, isNotEntered });
            else
                throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                         .arg(buf[i]).arg(line).toStdString());
        }
        else
            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                     .arg(buf[i]).arg(line).toStdString());
    }
}

void parseConstants(const QString& block, int& line)
{
    QString buf = block.mid(block.indexOf('{') + 1, block.indexOf('}') - block.indexOf('{') - 1);

    for (int i = 0; i < buf.length(); i++)
    {
        if (buf[i].isSpace())
        {
            if (buf[i] == '\n')
                line++;
        }
        else if (buf[i].isLetter() || buf[i] == '_')
        {
            QString constName(buf[i++]);
            QString constDescription;
            QString constValue;

            while (i < buf.length() && (buf[i].isLetterOrNumber() || buf[i] == '_'))
                constName += buf[i++];

            skipSpaces(buf, i, line);

            if (buf[i] == ':')
                constDescription = parseCommandOrDescriptionOrName(buf, i, line);

            if (buf[i] == '=')
            {
                constValue = parseValue(buf, i, line);

                if (!constValue.isEmpty() && buf[i] == ';')
                    parsed.constants.push_back({ constName, constDescription, constValue });
                else
                {
                    if (constValue.isEmpty())
                    {
                        throw std::runtime_error(QString("A constant must be declared with a value assigned: %1, line %2.")
                                                 .arg(constName).arg(line).toStdString());
                    }
                    else
                        throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                 .arg(buf[i]).arg(line).toStdString());
                }
            }
            else
                throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                         .arg(buf[i]).arg(line).toStdString());
        }
        else
            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                     .arg(buf[i]).arg(line).toStdString());
    }
}

void parseBlocks(const QString& block, int& line)
{
    QString buf = block.mid(block.indexOf('{') + 1, block.indexOf('}') - block.indexOf('{') - 1);
    bool isExpectedBlock = true;
    bool inBlock = false;

    for (int i = 0; i < buf.length(); i++)
    {
        if (buf[i].isSpace())
        {
            if (buf[i] == '\n')
                line++;
        }
        else if (buf[i] == '[')
        {
            inBlock = true;

            QString blockName;
            QString blockDescription;
            QList<Command> blockCommands;

            isExpectedBlock = false;
            i++;

            while (i < buf.length() && buf[i] != ']')
            {
                if (buf[i].isSpace())
                {
                    if (buf[i++] == '\n')
                        line++;
                }
                else if (buf[i].isLetter())
                {
                    QString word(buf[i++]);

                    while (buf[i].isLetter())
                        word += buf[i++];

                    if (word == "name")
                    {
                        skipSpaces(buf, i, line);

                        if (buf[i] != ':')
                            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        blockName = parseCommandOrDescriptionOrName(buf, i, line);

                        if (buf[i] != ';')
                            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        i++;
                    }
                    else if (word == "description")
                    {
                        skipSpaces(buf, i, line);

                        if (buf[i] != ':')
                            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        blockDescription = parseCommandOrDescriptionOrName(buf, i, line);

                        if (buf[i] != ';')
                            throw std::runtime_error(QString("Error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        i++;
                    }
                    else if (word == "commands")
                    {
                        if (buf[i] != ':')
                            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        i++;
                        skipSpaces(buf, i, line);

                        if (buf[i] != '(')
                            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                     .arg(buf[i]).arg(line).toStdString());

                        i++;
                        skipSpaces(buf, i, line);

                        while (i < buf.length() && buf[i] != ')')
                        {
                            if (buf[i].isSpace())
                            {
                                if (buf[i++] == '\n')
                                    line++;
                            }
                            else if (buf[i].isLetter() || buf[i] == '_')
                            {
                                QString command;
                                QString description;
                                QString resultVariable(buf[i++]);

                                while (i < buf.length() && (buf[i].isLetterOrNumber() || buf[i] == '_'))
                                    resultVariable += buf[i++];

                                skipSpaces(buf, i, line);

                                if (buf[i] != '=')
                                    throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                             .arg(buf[i]).arg(line).toStdString());

                                command = parseCommandOrDescriptionOrName(buf, i, line);

                                if (buf[i] == ':')
                                    description = parseCommandOrDescriptionOrName(buf, i, line);

                                if (buf[i] == ';')
                                    blockCommands.push_back(Command(command, description, resultVariable));
                                else
                                    throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                             .arg(buf[i]).arg(line).toStdString());

                                i++;
                            }
                            else if (buf[i] == '"')
                            {
                                QString command;
                                QString description;

                                i++;
                                while(i < buf.length() && buf[i] != '"')
                                    command += buf[i++];

                                i++;

                                while (i < buf.length() && buf[i].isSpace())
                                    if (buf[i++] == '\n')
                                        line++;

                                if (buf[i] == ':')
                                    description = parseCommandOrDescriptionOrName(buf, i, line);

                                if (buf[i] == ';')
                                    blockCommands.push_back(Command(command, description));
                                else
                                    throw std::runtime_error(QString("Error: \"%1\" is an incorrect symbol, line %2.")
                                                             .arg(buf[i]).arg(line).toStdString());

                                i++;
                            }
                            else
                                throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                         .arg(buf[i]).arg(line).toStdString());
                        }

                        if (i >= buf.length() || buf[i] != ')')
                            throw std::runtime_error(QString("Syntax error: ')' is not found, line %1.")
                                                     .arg(line).toStdString());

                        i++;
                        skipSpaces(buf, i, line);

                        if (i >= buf.length() || buf[i] != ']')
                            throw std::runtime_error(QString("Syntax error: ']' is not found, line %1.")
                                                     .arg(line).toStdString());
                    }
                    else
                        throw std::runtime_error(QString("Error: \"%1\" is not a keyword, line %2.")
                                                 .arg(buf).arg(line).toStdString());
                }
                else
                    throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                             .arg(buf[i]).arg(line).toStdString());
            }

            parsed.blocks.push_back({ blockName, blockDescription, blockCommands });
        }
        else if (buf[i] == ']')
        {
            if (!inBlock)
                throw std::runtime_error(QString("Syntax error: \"]\" is an incorrect symbol, line %1.")
                                         .arg(line).toStdString());
            inBlock = false;
        }
        else if (buf[i] == ',')
        {
            isExpectedBlock = true;
        }
        else
            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                     .arg(buf[i]).arg(line).toStdString());
    }

    if (isExpectedBlock)
        throw std::runtime_error(QString("Error: block was expected, line %1.")
                                 .arg(line).toStdString());
}

void parseBlock(const QString& block, int& line)
{
    if (block.startsWith("Variables"))
        parseVariables(block, line);
    else if (block.startsWith("Constants"))
        parseConstants(block, line);
    else if (block.startsWith("Blocks"))
        parseBlocks(block, line);
    else
        throw std::runtime_error(QString("Error, line %1.").arg(line).toStdString());
}

const Parsed ModuleParser::parse(const QString& source)
{
    QString buf;
    int line = 1;

    for (int i = 0; i < source.length(); i++)
    {
        if (source[i].isSpace())
        {
            if (source[i] == '\n')
                line++;
        }
        else if (source[i].isLetter())
        {
            buf = source[i++];
            while (i < source.length() && source[i].isLetter())
                buf += source[i++];

            if (!isKeyword(buf))
            {
                throw std::runtime_error(QString("Error: \"%1\" is not a keyword, line %2.")
                                         .arg(buf).arg(line).toStdString());
            }
            else
            {
                int blockBeginingLine = line;

                while (i < source.length() && source[i] != '{')
                {
                    if (source[i].isSpace())
                    {
                        if (source[i++] == '\n')
                            line++;
                    }
                    else
                        throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                                 .arg(source[i]).arg(line).toStdString());
                }

                if (i >= source.length() || source[i] != '{')
                    throw std::runtime_error(QString("Syntax error: \"{\" is not found, block starts on line %1.")
                                             .arg(blockBeginingLine).toStdString());

                while (i < source.length() && source[i] != '}')
                {
                    if (source[i] == '\n')
                        line++;
                    buf += source[i++];
                }

                if (i >= source.length() || source[i] != '}')
                    throw std::runtime_error(QString("Syntax error: \"}\" is not found, block starts on line %1.")
                                             .arg(blockBeginingLine).toStdString());
                else
                    parseBlock((buf + source[i]), blockBeginingLine);
            }
        }
        else
            throw std::runtime_error(QString("Syntax error: \"%1\" is an incorrect symbol, line %2.")
                                     .arg(source[i]).arg(line).toStdString());
    }

    return parsed;
}
