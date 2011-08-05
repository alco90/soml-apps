#!/usr/bin/env ruby

class Application
  attr_reader :name, :bin, :lib, :link

  def initialize(name, link = nil, lib = nil)
    @name = name
    @bin = "oml2-#{name}"
    @lib = lib
    @link = link || "#{name}_oml2"
  end

  def plan
    files = []
    files << "/usr/bin/#{@bin}"
    files << "/usr/bin/#{@link}"
    files << "/usr/lib/#{@name}/#{@lib}" unless @lib.nil?
    files
  end
end

class Package
  attr_reader :name, :apps

  @@packages = Hash.new

  def initialize(name, &block)
    @apps = []
    @name = name
    @@packages[name] = self
    yield self unless block.nil?
  end

  def defApplication(name, link = nil, lib = nil)
    @apps << Application.new(name, link, lib)
  end

  def self.packages
    @@packages
  end

  def package_name
    "oml2-#{@name}"
  end

  def plan
    curdir = ENV['CURDIR']
    cmds = []
    cmds << "mkdir -p #{curdir}/debian/#{package_name}/usr/bin"
    cmds << "mkdir -p #{curdir}/debian/#{package_name}/usr/lib/#{name}"

    @apps.each do |a|
      a.plan.each do |file|
        puts "Building plan for #{file}"
        if file[/\/usr\/lib/].nil? then
          # not a lib file
          cmds << "mv #{curdir}/debian/tmp#{file} #{curdir}/debian/#{package_name}#{file}"
        else
          # find the lib name, e.g. 'libotg2', and the libdir, e.g. '/usr/lib/otg2'
          lib = file.split('/')[-1]
          libdir = file.split('/')[0..-2].join('/')
          puts "LIB = #{lib}"
          puts "LIBDIR = #{libdir}"
          cmds << "mv #{curdir}/debian/tmp#{file}.so.* #{curdir}/debian/#{package_name}#{libdir}"
          cmds << "mv #{curdir}/debian/tmp#{file}.a #{curdir}/debian/#{package_name}#{libdir}"
          cmds << "mv #{curdir}/debian/tmp#{file}.la #{curdir}/debian/#{package_name}#{libdir}"
        end
      end
    end
    cmds
  end
end

# Create a package.  The block gets the Package object as its parameter
def defPackage(name, &block)
  Package.new(name, &block)
end

# Bare defApplication's create a package of the same name
def defApplication(name, link = nil, lib = nil)
  Package.new(name).defApplication(name, link, lib)
end

defApplication("gpslogger")
defApplication("nmetrics")
defApplication("trace")
defApplication("wlanconfig")

defPackage("otg2") do |p|
  p.defApplication("otg2", "otg2", "libotg2")
  p.defApplication("otr2", "otr2")
end

def run
  app = Application.new("nmetrics")
  p app.plan

  cmds = []
  Package.packages.each_value { |q| cmds = cmds + q.plan }

  cmds.each { |c| p c; `#{c}` }
end

if __FILE__ == $PROGRAM_NAME then run; end
