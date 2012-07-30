module ConfigurationReader

  # Tries to find a configuration file
  # @return the hashtable with the configuration values if any found, nil otherwise
  def self.look_for_configuration(possible_names, find_path, warn_not_found = true)
    configurationFound = false
    possible_names.each do |possible_name|
      path = File.join(find_path, possible_name)

      if (File.exists?(path))
        configurationFound = true
        # Read config

        config = {}
        namespace = "global"
        File.open(path).each do |line|
          next if line =~ /^(#.*|[\s]+)$/ # Comment or new line
          
          if line =~ /^\[([\w_\-0-9]+)\][\s]*$/ # New namespace
            namespace = $1
            puts "Found #{namespace}" if $g_verbose
            next
          end

          if line =~ /^([\w_\-\.]+)[\s]+([\d\.]+|yes|no|".*"|'.*'|\#[\dABCDEFabcdef]{6})[\s]*$/
            key = $1
            value = $2

            # Strip the quotes if a string and replace spaces with _
            if value =~ /^("|')(.*)("|')$/
              value = $2
              value.gsub!(" ", "_")
            end

            # Convert hexdecimal string to int if a color
            if value =~ /^\#([\dABCDEFabcdef]{6})$/
              hex = "00" + $1  # Pad it to fit a 32 bit int
              value = hex.to_i(16)
            end

            config[namespace + '.' + key] = value
            next
          else
            # Invalid syntax
            puts "ERROR! Syntax error: #{line} is not a valid configuration statement on #{path}"
            return nil
          end
        end

        return config;
      end
    end

    puts "ERROR! Cannot find a valid configuration file (#{possible_names.join(' - OR - ')}) in #{find_path}" if !configurationFound and warn_not_found
    nil
  end
end
