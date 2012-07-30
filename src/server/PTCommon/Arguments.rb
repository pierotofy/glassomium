# Configuration file

require './PTCommon/ArgumentParser.rb'

module PTCommon

  class Arguments

    # Set default values, then merge with command line arguments, then create accessors for each
    def initialize(defaults)
      conf = defaults
       params = ArgumentParser.parse(ARGV)
       conf = conf.merge(params);

      # Define runtime accessors method for each entry
       conf.each { |key, value|
         self.class.send(:define_method, key.gsub(/-/,"_")) {
           value
         }
       }
    end
  end
end