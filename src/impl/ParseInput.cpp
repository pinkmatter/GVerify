/*
* Copyright 2018 Pinkmatter Solutions
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ParseInput.h"

#include "ul_Logger.h"
#include "ul_Utility.h"
#include "ul_File.h"

SArgs::SArgs()
{
    chipSize = 33;
    amountOfPyramids = 1;
    chipGenType = ultra::EChips::CHIP_GEN_EVEN;
    nullValue = 0;
    mayContainNullValues = true;
    resampleType = ultra::EResamplerEnum::RESAMPLE_TYPE_BI;
    saveTiff = false;
    saveUnion = true;
}

SArgs::~SArgs()
{

}

SArgs::SImageItem::SImageItem()
{
    pathToImage = "";
    proj4Str = "";
}

SArgs::SImageItem::~SImageItem()
{
    pathToImage = "";
    proj4Str = "";
}

std::ostream &operator<<(std::ostream &stream, const SArgs::SImageItem &o)
{
    stream << "\tpathToImage = '" << o.pathToImage << "'" << std::endl;
    stream << "\tproj4Str = '" << o.proj4Str << "'";

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const SArgs &o)
{
    stream << "gridSize = '" << o.chipGenerationGridSize << "'" << std::endl;
    stream << "resampleType  = '" << ultra::EResamplerEnum::resamplingTypeToStr(o.resampleType) << "'" << std::endl;
    stream << "nullValue = '" << o.nullValue << "'" << std::endl;
    stream << "mayContainNullValues = '" + ultra::boolToStr(o.mayContainNullValues) << "'" << std::endl;
    stream << "chipGenType = '" << ultra::EChips::chipGenTypeToStr(o.chipGenType) << "'" << std::endl;
    stream << "fixedChipLocationFile = '" << o.fixedChipLocationFile << std::endl;
    stream << "correlationThreshold = '" << o.correlationThreshold << "'" << std::endl;
    stream << "referenceImage = \n'" << o.referenceImage << "'" << std::endl;
    stream << "inputImage = \n'" << o.inputImage << "'" << std::endl;
    stream << "logPath = '" << o.logPath << "'" << std::endl;
    stream << "outputPath = '" << o.outputPath << "'" << std::endl;
    stream << "threadCount = '" << o.threadCount << "'" << std::endl;
    stream << "usePhaseCorrelation = '" << ultra::boolToStr(o.usePhaseCorrelation) << "'" << std::endl;
    stream << "workingDirectory = '" << o.workingDirectory << "'" << std::endl;
    stream << "chipSize = '" << o.chipSize << "'" << std::endl;
    stream << "amountOfPyramids = '" << o.amountOfPyramids << "'" << std::endl;
    stream << "saveTiff = '" << ultra::boolToStr(o.saveTiff) << "'" << std::endl;
    stream << "saveUnion = '" << ultra::boolToStr(o.saveUnion) << "'" << std::endl;
    return stream;
}

static int ValidateInputs(SArgs &args)
{
    if (args.logPath != "")
    {
        std::string logFilePath = args.logPath + "/image-gverify.log";
        if (ultra::CLogger::getInstance()->setLogFilePath(logFilePath) != 0)
        {
            std::cout << "Failed to create log file '" << logFilePath << "'" << std::endl;
            return 1;
        }
    }

    if (args.threadCount <= 0)
    {
        std::cout << "Thread count cannot be zero or less" << std::endl;
        return 1;
    }

    if (!ultra::pathExists(args.referenceImage.pathToImage))
    {
        std::cout << "Path '" << args.referenceImage.pathToImage << "' does not exist" << std::endl;
        return 1;
    }

    if (!ultra::pathExists(args.inputImage.pathToImage))
    {
        std::cout << "Path '" << args.inputImage.pathToImage << "' does not exist" << std::endl;
        return 1;
    }

    if (!ultra::CFile(args.outputPath).isDirectoryWritable())
    {
        std::cout << "Output folder does not exist or is not writable" << std::endl;
        return 1;
    }

    if (!ultra::CFile(args.workingDirectory).isDirectoryWritable())
    {
        std::cout << "Working directory does not exist or is not writable" << std::endl;
        return 1;
    }

    if (args.chipSize < 3)
    {
        std::cout << "Chip Size must be larger than 3 and must be odd" << std::endl;
        return 1;
    }
    if (args.chipSize % 2 != 1)
    {
        args.chipSize++;
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, "Chip Size must be odd adding 1 to make Chip Size odd");
    }

    if (args.amountOfPyramids < 1)
    {
        std::cout << "Pyramid levels must be an integer larger than 0" << std::endl;
        return 1;
    }

    if (args.chipGenType == ultra::EChips::CHIP_GEN_FIXED_LOCATION)
    {
        if (!ultra::CFile(args.fixedChipLocationFile).isFile())
        {
            std::cout << "Path to fixed-chip-location file '" << args.fixedChipLocationFile << "' does not exist or does not point to a file" << std::endl;
            return 1;
        }
    }

    return 0;
}

static int ParseChipType(ultra::EChips::EChipType &type, const std::string &second)
{
    ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, "Chip gen type is '" + second + "'");
    type = ultra::EChips::strToChipGenType(second);
    if (type == ultra::EChips::CHIP_GEN_TYPE_COUNT)
    {
        return 1;
    }
    return 0;
}

int ParseInput(int argc, char **argv, SArgs &args)
{
    if (argc % 2 != 1)
    {
        std::cout << "Invalid usage" << std::endl;
        return 1;
    }

    bool fail = false;
    bool gotGridSize = false;
    args.logPath = "";
    args.threadCount = 1;
    args.correlationThreshold = 0.75;
    args.usePhaseCorrelation = false;

    bool got[5] = {false};

    for (int x = 1; x < argc; x++)
    {
        std::string first = argv[x];
        std::string second = argv[++x];

        if (first == "-b")
        {
            args.originalReferencePath = args.referenceImage.pathToImage = second;
            got[0] = true;
        }
        else if (first == "-m")
        {
            args.originalInputPath = args.inputImage.pathToImage = second;
            got[1] = true;
        }
        else if (first == "-o")
        {
            args.outputPath = second;
            got[2] = true;
        }
        else if (first == "-w")
        {
            args.workingDirectory = second;
            got[3] = true;
        }
        else if (first == "-nv")
        {
            args.nullValue = atof(second.c_str());
            got[4] = true;
        }
        else if (first == "-n")
        {
            if (ultra::isInt(second))
            {
                args.threadCount = atoi(second.c_str());
            }
            else
            {
                std::cout << "Thread count must be an integer" << std::endl;
                return 1;
            }
        }
        else if (first == "-l")
        {
            args.logPath = second;
        }
        else if (first == "-c")
        {
            args.correlationThreshold = atof(second.c_str());
        }
        else if (first == "-r")
        {
            second = ultra::toUpper(second);
            if (second == "NN")
                args.resampleType = ultra::EResamplerEnum::RESAMPLE_TYPE_NN;
            else if (second == "BI")
                args.resampleType = ultra::EResamplerEnum::RESAMPLE_TYPE_BI;
            else if (second == "CI")
                args.resampleType = ultra::EResamplerEnum::RESAMPLE_TYPE_CI;
            else
            {
                std::cout << "Invalid resample technique '" << second << "'" << std::endl;
                return 1;
            }
        }
        else if (first == "-p")
        {
            if (ultra::isInt(second))
            {
                args.amountOfPyramids = atoi(second.c_str());
            }
            else
            {
                std::cout << "Pyramid levels must be an integer" << std::endl;
                return 1;
            }
        }
        else if (first == "-t")
        {
            if (ParseChipType(args.chipGenType, second) != 0)
            {
                std::cout << "Invalid chip type of '" << second << "'" << std::endl;
                return 1;
            }
        }
        else if (first == "-t_file")
        {
            args.fixedChipLocationFile = second;
        }
        else if (first == "-b_p")
        {
            args.referenceImage.proj4Str = second;
        }
        else if (first == "-m_p")
        {
            args.inputImage.proj4Str = second;
        }
        else if (first == "-cs")
        {
            if (ultra::isInt(second))
            {
                args.chipSize = atol(second.c_str());
            }
            else
            {
                std::cout << "Chip size must be an integer" << std::endl;
                return 1;
            }
        }
        else if (first == "-st")
        {
            args.saveTiff = ultra::strToBool(second);
        }
        else if (first == "-su")
        {
            args.saveUnion = ultra::strToBool(second);
        }
        else if (first == "-g")
        {
            gotGridSize = true;
            if (ultra::isInt(second))
            {
                args.chipGenerationGridSize = atol(second.c_str());
            }
            else
            {
                std::cout << "Grid size must be an integer" << std::endl;
                return 1;
            }
        }
        else if (first == "-usePhaseCorrelation")
        {
            if (ultra::toUpper(second) == "TRUE")
                args.usePhaseCorrelation = true;
            else if (ultra::toUpper(second) == "FALSE")
                args.usePhaseCorrelation = false;
            else
                return 1;
        }
        else
        {
            std::cout << "Invalid argument '" << first << "'" << std::endl;
            return 1;
        }
    }

    for (int x = 0; x < ultra::getArraySize(got); x++)
    {
        if (!got[x])
            fail = true;
    }

    if (!gotGridSize)
    {
        args.chipGenerationGridSize = args.chipSize * 2;
    }

    if (!fail)
    {
        if (ValidateInputs(args) != 0)
        {
            return 1;
        }
    }

    ultra::AUltraThreadPool::setDefaultPoolSize(args.threadCount);

    return ((fail) ? (1) : (0));
}

void showHelp(int argc, char **argv)
{
    std::cout << "Usable arguments:" << std::endl;
    std::cout << std::endl;
    std::cout << "\t" << "-b <reference image>" << std::endl;
    std::cout << "\t" << "-m <input image>" << std::endl;
    std::cout << "\t" << "-w <path to working directory>" << std::endl;
    std::cout << "\t" << "-o <output folder>" << std::endl;
    std::cout << "\t" << "-nv <null value>" << std::endl;

    std::cout << "\t" << "-t [chip gen type (EVEN / FIXED_LOCATION / SOBEL / HARRIS)]" << std::endl;
    std::cout << "\t" << "-t_file [path to fixed-chip-location file]" << std::endl;
    std::cout << "\t" << "-p [pyramid levels(1)]" << std::endl;
    std::cout << "\t" << "-n [thread count(1)]" << std::endl;
    std::cout << "\t" << "-l [log folder]" << std::endl;
    std::cout << "\t" << "-c [correlation coefficient(0.75)]" << std::endl;
    std::cout << "\t" << "-r [resample technique (NN/BI/CI)]" << std::endl;
    std::cout << "\t" << "-cs [chip size (33)]" << std::endl;
    std::cout << "\t" << "-st [save geotiff (false)]" << std::endl;
    std::cout << "\t" << "-su [save union images between reference and input (true)]" << std::endl;
    std::cout << "\t" << "-g [grid size (chip size * 2)]" << std::endl;
    std::cout << "\t" << "-b_p [reference image proj4 string (example \"+proj=utm +ellps=WGS84 +units=m +no_defs +zone=36\"')]" << std::endl;
    std::cout << "\t" << "-m_p [input image proj4 string (example \"+proj=eqc +ellps=sphere +R=5729577.951308 +units=m +no_defs\"')]" << std::endl;
    std::cout << "\t" << "-usePhaseCorrelation [true/false (false)]" << std::endl;
    std::cout << std::endl;
    std::cout << "Note:" << std::endl;
    std::cout << "\t'-b_p' and '-m_p' do not need to be specified, these are only used if you wish to override" << std::endl;
    std::cout << "\tthe detected Proj4 projection strings" << std::endl;
    std::cout << std::endl;
    std::cout << "fixed-chip-location structure:" << std::endl;
    std::cout << "  Is a plain text file on which each line must contain a latitude,longitude decimal degrees pair in WGS84" << std::endl;
    std::cout << "  Example:" << std::endl;
    std::cout << "  -25.32,132.3" << std::endl;
    std::cout << "  -23.23,133.343" << std::endl;
    std::cout << "  ..." << std::endl;
    std::cout << "  -25.342,134.33" << std::endl;
}
