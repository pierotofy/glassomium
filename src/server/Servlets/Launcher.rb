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
