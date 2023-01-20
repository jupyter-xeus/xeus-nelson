/***************************************************************************
* Copyright (c) 2023, Hind Montassif                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/


#ifndef XEUS_NELSON_INTERPRETER_HPP
#define XEUS_NELSON_INTERPRETER_HPP

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "xeus_nelson_config.hpp"
#include "xeus/xinterpreter.hpp"

#include "MainEvaluator.hpp"
#include "WarningEmitter.h"
#include "ErrorEmitter.h"
#include "NelsonPrint.hpp"
#include "EvaluateCommand.hpp"
#include "characters_encoding.hpp"
#include "BuiltInFunctionDefManager.hpp"
#include "addpathBuiltin.hpp"
#include "Nelson_VERSION.h"

#include "VariableCompleter.hpp"
#include "MacroCompleter.hpp"
#include "FileCompleter.hpp"
#include "BuiltinCompleter.hpp"

#include "LexerInterface.hpp"

namespace nl = nlohmann;

namespace xeus_nelson
{
    class XEUS_NELSON_API interpreter : public xeus::xinterpreter
    {
    public:

        interpreter();
        virtual ~interpreter();

    protected:

        void configure_impl() override;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

    private:

        Nelson::Evaluator * m_evaluator;
    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
