
defApplication('app:gps', 'gps') do |a|

  a.version(1, 0, 0)
  a.shortDescription = 'Application to communicate to GPSD'
  a.description = %{
This application allows to receive information from gpsd
}

  a.defProperty('address', 'address of gpsd', ?a, 
		:type => :string)
  a.defProperty('port', 'port of gpsd', ?p, 
		:type => :int, :default => 2947)
  a.defProperty('sample-interval', 
				'Time between consecutive measurements [sec]', ?s, 
				:type => :int, :unit => 'seconds', :default => 1,
				:impl => { :var_name => 'sample_interval' })

  a.defMeasurement("gps_information") do |m|
      m.defMetric('latitude', 'string', ' Latitude of the node')
      m.defMetric('longitude', 'string', ' Longitude of the node')
 end
end
