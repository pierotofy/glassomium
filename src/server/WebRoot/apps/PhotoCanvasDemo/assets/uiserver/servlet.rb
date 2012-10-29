require './Servlets/ServletPlugin'
require "#{File.dirname(__FILE__)}/PictureScanner"

require 'webrick'
include WEBrick

#Current list of images
$images = ["/Capture.PNG"]

$changed_since_last_req = false

# Root physical disk location of the current list of images
$mount_point = ""

class ServletPlugin
	def get_servlet_handlers
	   {"servlet" => PhotoCanvasDemoServlet,
	    "photo" => PhotoCanvasDemoPictureServlet}
	end

	def initialize
		@picture_scanner = PictureScanner.new(self)
		@picture_scanner.start
	end

	# Delegate method called by PictureScanner
	def on_picture_list_changed(mount_point, images)
		$mount_point = mount_point
		$images = images
	end
end

class PhotoCanvasDemoServlet < WEBrick::HTTPServlet::AbstractServlet
	def initialize(server)
		super(server)
	end

    def do_GET(request, response)
      response.status = 200
      response['Content-Type'] = "text/html"

      response.body = case request.query['action']
      	when "getImageList"
      		response.body = $images.to_s
      	else
      		"Unknown action"
  	  end
    end
end

class PhotoCanvasDemoPictureServlet < WEBrick::HTTPServlet::AbstractServlet
	def initialize(server)
		super(server)
	end

    def do_GET(request, response)
  	  response.status = 200
  	  response['Content-Type'] = "text/html"

      # Find request URL
      image_url = request.query['id']

      if $mount_point != "" && image_url && $images.include?(image_url)
      	absolute_path = $mount_point + image_url
	  	response['Content-Type'] = get_mime(absolute_path)
	  	response['Content-Length'] = File.size(absolute_path)
      	response.body = open(absolute_path, "rb") {|io| io.read }
      else
      	response.body = "Not found!"
      end
    end


    def get_mime(path)
    	if path=~/^.*\.([\w]{3,4})$/
    		case $1.downcase
	    		when "bmp"
	    			"image/x-windows-bmp"
	    		when "png"
	    			"image/png"
	    		when "jpg"
	    			"image/jpeg"
	    		when "jpeg"
	    			"image/jpeg"
	    		when "gif"
	    			"image/gif"
	    		else
	    			""
    		end
    	else
    		""
    	end
    end
end