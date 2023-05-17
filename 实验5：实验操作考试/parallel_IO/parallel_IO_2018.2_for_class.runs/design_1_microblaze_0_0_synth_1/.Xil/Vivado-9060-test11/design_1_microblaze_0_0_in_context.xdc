set_property -quiet CLOCK_PERIOD_OOC_TARGET 10.000 [get_ports -no_traverse -quiet Clk]
set_property -quiet CLOCK_PERIOD_OOC_TARGET 33.333 [get_ports -no_traverse -quiet Dbg_Clk]
set_property -quiet CLOCK_PERIOD_OOC_TARGET 33.333 [get_ports -no_traverse -quiet Dbg_Update]
set_property -quiet IS_IP_OOC_CELL TRUE [get_cells -of [get_ports -no_traverse -quiet Byte_Enable[0]]]
