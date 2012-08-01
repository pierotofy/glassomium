=begin
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 The Glassomium Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
=end

module Servlets
  require 'webrick'
  require './ThemeManager'
  require './Servlets/BaseServlet'
  require './App'

  class Launcher < BaseServlet
  
    def get_body(request, response)
      ThemeManager::getTemplate("launcher/main"){ |entry|
        case entry
          when "ITEM_LIST"
            getNavigatorItems
          else
            entry + " missing"
        end
      }
    end

    def getNavigatorItems
      result = ""
      $g_apps.each { |app_name, app|
        result += ThemeManager::getTemplate("launcher/item"){ |entry|
          case entry
          when "ITEM_NAME"
            app_name
          when "ITEM_URL"
            app.app_url
          when "ITEM_ICON"
            app.icon_url
          else
            entry + " missing"
          end
        }
      }      
      return result
    end

  end
end
