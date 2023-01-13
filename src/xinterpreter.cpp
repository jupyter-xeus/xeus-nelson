/***************************************************************************
* Copyright (c) 2023, Hind Montassif                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <string>
#include <vector>
#include <iostream>

#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xhelper.hpp"

#include "xeus-nelson/xinterpreter.hpp"

namespace nl = nlohmann;

namespace xeus_nelson
{
 
    interpreter::interpreter()
    {
        xeus::register_interpreter(this);

        // Initialize nelson evaluator
        m_evaluator = Nelson::createMainEvaluator(/*_mode =*/ BASIC_TERMINAL, /*lang =*/ L"en_US", /*minimizeWindow =*/ false); // L"en_US" for default lang
        if (m_evaluator != nullptr)
        {
            // TODO test if this is useless or not
            Nelson::setWarningEvaluator(m_evaluator);
            Nelson::setErrorEvaluator(m_evaluator);
            Nelson::setPrintInterface(m_evaluator->getInterface());
            Nelson::BuiltInFunctionDefManager::getInstance()->add("addpath",
                                                                  (Nelson::ptrBuiltin)Nelson::FunctionsGateway::addpathBuiltin,
                                                                  1, -1, L"", L"functions_manager",                                              (size_t)Nelson::CPP_BUILTIN_WITH_EVALUATOR, true);
        }
    }

    nl::json interpreter::execute_request_impl(int execution_counter, // Typically the cell number
                                               const  std::string & code, // Code to execute
                                               bool silent,
                                               bool /*store_history*/,
                                               nl::json /*user_expressions*/,
                                               bool /*allow_stdin*/)
    {
        if (m_evaluator == nullptr)
        {
            const std::string evalue = "Nelson interpreter not initialized";
            std::vector<std::string> traceback({"Interpreter error: " + evalue});
            if(!silent)
            {
                publish_execution_error("Interpreter error", evalue, traceback);
            }
            // Compose error_reply message
            return xeus::create_error_reply("Interpreter error", evalue, traceback);
        }

        try
        {
            std::wstring output;
            const std::wstring& command = Nelson::utf8_to_wstring(code);
            Nelson::EvaluateConsoleCommandToString(m_evaluator, command, output);
            if (!silent)
            {
                nl::json pub_data;
                pub_data["text/plain"] = Nelson::wstring_to_utf8(output);
                // Publish the execution result
                publish_execution_result(execution_counter, std::move(pub_data), nl::json::object());
            }

            // TODO to set correctly?
            //const nl::json& payload = nl::json::array();
            // Compose successful_reply message
            return xeus::create_successful_reply(nl::json::array(), nl::json::object());
        }
        catch (Nelson::Exception& e)
        {
            const auto evalue = Nelson::wstring_to_utf8(e.getMessage());
            std::vector<std::string> traceback({"Interpreter error: " + evalue});
            if (!silent)
            {
                publish_execution_error("Interpreter error", evalue, traceback);
            }
            // Compose error_reply message
            return xeus::create_error_reply("Interpreter error", evalue, traceback);
        }
    }

    void interpreter::configure_impl()
    {
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        // Insert code here to validate the ``code``
        // and use `create_is_complete_reply` with the corresponding status
        // "unknown", "incomplete", "invalid", "complete"
        return xeus::create_is_complete_reply("complete"/*status*/, "   "/*indent*/);
    }

    nl::json interpreter::complete_request_impl(const std::string&  code,
                                                     int cursor_pos)
    {
        // Should be replaced with code performing the completion
        // and use the returned `matches` to `create_complete_reply`
        // i.e if the code starts with 'H', it could be the following completion
       
//         return xeus::create_complete_reply(
//             {
//                 std::string("Hello"), 
//                 std::string("Hey"), 
//                 std::string("Howdy")
//             },          /*matches*/
//             5,          /*cursor_start*/
//             cursor_pos  /*cursor_end*/
//         );

        return xeus::create_complete_reply(
            nl::json::array(),  /*matches*/
            cursor_pos,         /*cursor_start*/
            cursor_pos
        );
    }

    nl::json interpreter::inspect_request_impl(const std::string& /*code*/,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
    {
        
        return xeus::create_inspect_reply(true/*found*/, 
            {{std::string("text/plain"), std::string("hello!")}}, /*data*/
            {{std::string("text/plain"), std::string("hello!")}}  /*meta-data*/
        );
         
    }

    void interpreter::shutdown_request_impl()
    {
    }

    nl::json interpreter::kernel_info_request_impl()
    {

        const std::string  protocol_version = xeus::get_protocol_version();
        const std::string  implementation = "xeus-nelson";
        const std::string  implementation_version = XEUS_NELSON_VERSION;
        const std::string  language_name = "nelson";
        const std::string  language_version = NELSON_VERSION_STRING;
        const std::string  language_mimetype = "text/x-nelsonsrc";;
        const std::string  language_file_extension = "nelson";;
        const std::string  language_pygments_lexer = "";
        const std::string  language_codemirror_mode = "";
        const std::string  language_nbconvert_exporter = "";

        /* The jupyter-console banner for xeus-nelson is the following:
          __  _____ _   _ ___ 
          \ \/ / _ \ | | / __|
           >  <  __/ |_| \__ \
          /_/\_\___|\__,_|___/

          xeus-nelson: a Jupyter Kernel based on nelson
        */

        std::string banner = ""
              "  __  _____ _   _ ___\n"
              "  \\ \\/ / _ \\ | | / __|\n"
              "   >  <  __/ |_| \\__ \\\n"
              "  /_/\\_\\___|\\__,_|___/\n"
              "\n"
              "  xeus-nelson: a Jupyter Kernel based on nelson\n"
              "  Nelson";
        banner.append(language_version);

        const bool         debugger = false;
        const nl::json     help_links = nl::json::array();

        return xeus::create_info_reply(
            protocol_version,
            implementation,
            implementation_version,
            language_name,
            language_version,
            language_mimetype,
            language_file_extension,
            language_pygments_lexer,
            language_codemirror_mode,
            language_nbconvert_exporter,
            banner,
            debugger,
            help_links
        );
    }
}
