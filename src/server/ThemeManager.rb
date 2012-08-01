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

module ThemeManager
  require './ConfigurationReader'

  def self.setCurrentTheme(theme)
    @theme = theme
  end

  def self.getCurrentTheme
    @theme
  end
  
   def self.getTemplateContent(name)
    ["html", "htm", "tml"].each do |extension|
      filename = "Themes/#{@theme}/system/#{name}.#{extension}"

      if File.exists?(filename)
        return File.read(filename)
      end
    end

    return "404 NOT FOUND!"
  end

  def self.getTemplate(name)
    content = self.getTemplateContent(name)

    if block_given?
      content = content.gsub(/\{([\w_\d]+)\}/){
        yield $1
      }
    end

    return content
  end
  
  def self.themeExists?(theme)
    return File.exists?("Themes/#{theme}")
  end

  # Makes sure the configuration is present and readable
  def self.verify
    if self.readConfiguration == nil
      puts "ERROR! Could not verify theme, check your configuration files and try again."
      exit(1)
    end
  end

  def self.readConfiguration
    configuration = ConfigurationReader.look_for_configuration(["theme.cfg"], "./Themes/#{@theme}/")
    configuration
  end
end
