require './Servlets/ServletPlugin'
require "#{File.dirname(__FILE__)}/mp3info"

require 'webrick'
include WEBrick

class ServletPlugin
	def get_servlet_handlers
	   {"playlist" => PlaylistServlet}
	end

	def initialize
	end
end

class PlaylistServlet < WEBrick::HTTPServlet::AbstractServlet
	def initialize(server)
		super(server)
	end

    def do_GET(request, response)
      response.status = 200
      response['Content-Type'] = "text/html"

      # Enum files
	  result = '{"playlist":['
	  c = 0
	  Dir.glob(File.join("#{File.dirname(__FILE__)}/../../music/", '*.mp3')).each do |file|
  		Mp3Info.open(file) do |mp3|
  		  filename = file.gsub("#{File.dirname(__FILE__)}/../../", "")
  		  title = mp3.tag.title || filename.gsub("music/", "")
  		  artist = mp3.tag.artist || "Unknown artist"
  		  album = mp3.tag.artist || "Unknown album"

		  result += <<-EOS
{
    "#{c}": {"src":"#{filename}", "type":"audio/mp3"},
    "config": {
		"title": "#{title}",
		"artist": "#{artist}",
		"album": "#{album}",
		"poster": ""
    }
},
EOS

		  c = c + 1
		end
	  end

	  # remove last comma
	  result = result[0..-3]

	  result += ']}'

      response.body = result
    end
end