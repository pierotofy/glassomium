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
