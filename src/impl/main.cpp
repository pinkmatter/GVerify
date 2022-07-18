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
#include "GenerateGcps.h"
#include "GenerateResults.h"
#include "ReprojectImages.h"

static std::string VERSION = "0.25e";

int main(int argc, char **argv)
{
    ultra::CLogger::getInstance()->setLogLevel(ultra::CLogger::LOG_INFO);
    ultra::CLogger::getInstance()->LogLogLevels();
    SArgs args;
    ultra::CVector<ultra::SChipCorrelationResult> result;

    if (ParseInput(argc, argv, args) != 0)
    {
        showHelp(argc, argv);
        return 1;
    }
    ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, "Version " + VERSION);
    ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, "Arguments\n" + ultra::toString(args) + "\n\n");

    if (ReprojectImages(args) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from ReprojectImages()");
        return 1;
    }
    bool overlapExists = false;
    if (GenerateGcps(args, result, overlapExists) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from GenerateGcps()");
        return 1;
    }

    if (GenerateResults(args, result, overlapExists) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from GenerateResults()");
        return 1;
    }

    return 0;
}
