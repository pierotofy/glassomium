module Servlets
  require 'webrick'
  require './Servlets/BaseServlet'
  require 'yaml'

  class SystemStatic < BaseServlet
    def initialize(server)
      super(server)
    end

    def get_body(request, response)
      path = request.path_info.gsub('..', '')

      ThemeManager::getTemplate("#{path}/index")
    end
  end
end
