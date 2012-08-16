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

require './App'
require './ConfigurationReader'

class AppScanner
	
	# Goes through the specified folder
	# and tries to find apps
	# @return a list of valid apps
	def self.scanApps(folder, prefix_url)
		result = {}
		Dir.foreach(folder) do |current_dir|
			current_path = File.join(folder, current_dir)
			if File.directory?(current_path) && !([".","..",".svn"].include?(current_dir))

				# All subdirectories should be apps, but we need to look for certain files
				index_url = AppScanner.look_for(["index.html", "index.htm"], current_path, current_dir, prefix_url)
				next if !index_url

				icon_url = AppScanner.look_for(["assets/icon-512x512.png", "assets/icon-256x256.png", "assets/icon.png"], 
													current_path, current_dir, prefix_url)
				next if !icon_url

				configuration = ConfigurationReader.look_for_configuration(["assets/application.cfg", "assets/app.cfg"], current_path, false)
				configuration = {} if configuration == nil

				servlet_file = AppScanner.look_for(["assets/uiserver/servlet.rb"], current_path, current_dir, "WebRoot/apps/")

				result[current_dir] = App.new(index_url, icon_url, configuration, servlet_file)
				puts "Found #{current_dir}: #{index_url}, #{icon_url}, #{configuration}, #{servlet_file}" if $g_verbose
			end
		end

		return result
	end

	def self.look_for(possible_names, current_path, current_dir, prefix_url)
		found_url = nil
		possible_names.each do |possible_name|
			url = File.join(current_path, possible_name)
			if File.exists?(url)
				found_url = prefix_url + current_dir + "/" + possible_name
				break
			end
		end

		return found_url
	end
	
end
