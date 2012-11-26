# This is an OMF experiment that makes use of the OML4R pingWrap.rb
# ensure that the pingWrap.rb script is installed on your OMF node at /root/pingWrap.rb
# also make sure the oml4r gem is installed

defProperty('source', "node1-1.grid.orbit-lab.org", "ID of a resource")
defProperty('sink', "node1-2.grid.orbit-lab.org", "ID of a resource")
defProperty('sinkaddr', 'node1-2.grid.orbit-lab.org', "Ping destination address")

defApplication('ping_app', 'pingmonitor') do |a|
    a.path = "/root/pingWrap.rb"
    a.version(1, 2, 0)
    a.shortDescription = "Wrapper around ping"
    a.description = "ping application"
    a.defProperty('dest_addr', 'Address to ping', 'a', {:type => :string, :dynamic => false})
    a.defProperty('count', 'Number of times to ping', 'c', {:type => :integer, :dynamic => false})
    a.defProperty('interval', 'Interval between pings in s', 'i', {:type => :integer, :dynamic => false})

    a.defMeasurement('myping') do |m|
     m.defMetric('dest_addr',:string)
     m.defMetric('ttl',:int)
     m.defMetric('rtt',:float)
     m.defMetric('rtt_unit',:string)
   end
end

defGroup('Source', property.source) do |node|
  node.addApplication("ping_app") do |app|
    app.setProperty('dest_addr', property.sinkaddr)
    app.setProperty('count', 5)
    app.setProperty('interval', 1)
    app.measure('myping', :samples => 1)
  end
end

defGroup('Sink', property.sink) do |node|
end

onEvent(:ALL_UP_AND_INSTALLED) do |event|
  info "Starting the ping"
  group('Source').startApplications
  wait 6
  info "Stopping the ping"
  group('Source').stopApplications
  Experiment.done
end
