module Servlets
  require 'webrick'
  require './ThemeManager'
  require './Servlets/BaseServlet'
  require './App'

  class ApplicationConfigs < BaseServlet
   
    def get_body(request, response)
      result = ""
      $g_apps.each do |app_name, app|
        result += "APP_CONFIG_BEGIN\n#{app.app_url}\n"
        app.configuration.each do |nskey, value|
          parts = nskey.split('.')
          (namespace, key) = parts[0], parts[1..parts.length].join('.')
          result += "#{key} #{value}\n"
        end
        result += "APP_CONFIG_END\n"
      end

      result
    end

  end
end
