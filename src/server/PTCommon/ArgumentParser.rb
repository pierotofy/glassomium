# Argument parser class
# Takes the ARGV list and returns an hashed item of the form:
# { param1 (without -- or -) : value }
module PTCommon
  class ArgumentParser
      def self.parse(args)
        res = {}
        (0..args.length).each { |i|
          if args[i]=~/^--/
            if args[i+1] =~/^-/ || args[i+1] == nil
             res.store(String(args[i]).gsub(/--/, ""), true)
            elsif
             res.store(String(args[i].gsub(/--/, "")), args[i+1])
            end            
          elsif args[i]=~/^-[^-]/
            args[i].each_char { |c|
              next if c == '-'
              res.store(String(c), true)
            }            
          end
        }
        return res
      end
  end
end

