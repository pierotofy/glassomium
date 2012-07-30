module Servlets
  require 'webrick'
  require './ThemeManager'
  require './Servlets/BaseServlet'

  class ThemeConfig < BaseServlet
   
    def initialize(server)
      super(server)     

      @theme_cfg = ThemeManager.readConfiguration
    end

    def get_body(request, response)
      result = ""
      result += "THEME_CONFIG_BEGIN\n"
      @theme_cfg.each do |nskey, value|
        parts = nskey.split('.')
        (namespace, key) = parts[0], parts[1..parts.length].join('.')
        result += "#{namespace} #{key} #{value}\n"
      end
      result += "THEME_CONFIG_END\n"

      result
    end

  end
end
