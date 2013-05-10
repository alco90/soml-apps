# This file was automatically generated by oml2-scaffold 2.10.0
# The syntax of this file is documented at [0].
#
# [0] http://doc.mytestbed.net/doc/omf/OmfEc/Backward/AppDefinition.html

defApplication('oml:app:wattsup-oml2', 'wattsup-oml2') do |app|

  app.version(1, 0, 0)
  app.shortDescription = 'A program for interfacing with the Watts Up? Power Meter'

  app.description = %{The Watts Up? Pro Monitor allows to monitor the following
metrics: current watts, minimum watts, maximum watts, power factor, cumulative
watt hours, average monthly kilowatt hours, tier 2 kilowatt hour threshold (used
to calculate secondary kWh rates), elapsed time, cumulative cost, average
monthly cost, line volts, minimum volts, maximum volts, current amps,
minimum amps, and maximum amps.

This application reads these metrics and reports them using OML.
  }

  app.path = "@bindir@/wattsup-oml2"

  # Command line options supported by the un-instrumented application,
  # as obtained with wattsup --help
  app.defProperty('help', 'Display help text and exit', '-h',
        :type => :boolean, :order => 1)
  app.defProperty('version', 'Display version information and exit', '-V',
        :type => :boolean, :order => 1)
  app.defProperty('debug', 'Print out debugging messages', '-d',
        :type => :boolean, :order => 1)
  app.defProperty('count', 'Specify number of data samples', '-c',
        :type => :integer, :order => 1)
  app.defProperty('final', 'Print final interval information', '-z',
        :type => :boolean, :order => 1)
  app.defProperty('delim', 'Set field delimiter', '-f',
        :type => :string, :default => ", ", :order => 1)
  app.defProperty('newline', 'Use '\n' as delimter instead', '-n',
        :type => :boolean, :order => 1)
  app.defProperty('localtime', 'Print localtime with each data reading', '-t',
        :type => :boolean, :order => 1)
  app.defProperty('gmtime', 'Print GMT time with each data reading', '-g',
        :type => :boolean, :order => 1)
  app.defProperty('label', 'Show labels of each field', '-l',
        :type => :boolean, :order => 1)
  app.defProperty('suppress', 'Suppress printing of the field description', '-s',
        :type => :boolean, :order => 1)
  app.defProperty('calibrate', 'Print calibration parameters', '-b',
        :type => :boolean, :order => 1)
  app.defProperty('header', 'Print data field names (as read from device)', '-r',
        :type => :boolean, :order => 1)
  app.defProperty('interval', 'Get/Set sampling interval', '-i',
        :type => :integer, :order => 1)
  app.defProperty('mode', 'Get/Set display mode', '-m',
        :type => :integer, :order => 1)
  app.defProperty('user', 'Get/Set user parameters', '-u',
        :type => :string, :order => 1)
  app.defProperty('show-all', 'Show all device parameters', '-a',
        :type => :boolean, :order => 1)
  app.defProperty('no-data', 'Don\'t read any data (just read device info)', '-N',
        :type => :boolean, :order => 1)
  app.defProperty('set-only', 'Set parameters only (don\'t read them back)', '-S',
        :type => :boolean, :order => 1)
  app.defProperty('device', 'Serial port the device is connected at', '',
        :type => :string, :default => '/dev/ttyUSB0', :order => 2)
  app.defProperty('value', 'Specifies which of these to print out', '',
        :type => :string, :default => 'ALL', :order => 3)

  # Declare measurement points; generate OML injection helpers with
  #  oml2-scaffold --oml wattsup-oml2.rb
  app.defMeasurement("sensor") do |mp|
    mp.defMetric('val', :int32)
    mp.defMetric('inverse', :double)
    mp.defMetric('name', :string)
  end

  # Declare a giant Measurement Point showing all supported types
  app.defMeasurement("example") do |mp|
    mp.defMetric('boolean_field', :boolean)
    mp.defMetric('string_field', :string)
    mp.defMetric('int32_field', :int32)
    mp.defMetric('uint32_field', :uint32)
    mp.defMetric('int64_field', :int64)
    mp.defMetric('uint64_field', :uint64)
    mp.defMetric('double_field', :double)
    mp.defMetric('blob_field', :blob)
    mp.defMetric('guid_field', :guid)
  end

end

# Local Variables:
# mode:ruby
# End:
# vim: ft=ruby:sw=2
