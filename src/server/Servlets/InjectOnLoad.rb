module Servlets
  require 'webrick'
  require './Servlets/BaseServlet'

  class InjectOnLoad < BaseServlet
    def initialize(server)
      super(server)
    end

    def get_body(request, response)
      File.read("JS/injectOnLoad.js")
    end
  end
end
