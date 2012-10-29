require './Servlets/ServletPlugin'

require 'webrick'
include WEBrick

class ServletPlugin
	def get_servlet_handlers
	   {"getDate" => TestServlet}
	end

	def initialize
		# Nothing to initialize here
	end
end

class TestServlet < WEBrick::HTTPServlet::AbstractServlet
	def initialize(server)
		super(server)
	end

    def do_GET(request, response)
      response.status = 200
      response['Content-Type'] = "text/html"
      response.body = Time.new.inspect
    end
end