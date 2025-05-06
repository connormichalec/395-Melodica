# 395-Melodica

Current setup, Key decoding (and to midi) on the smaller stm chip, looping jankily combined on bigger stm chip that was originally just going to be used for synthesis. Smaller ship sends midi data to synthesis. Synthesis manages looping and sends back midi data for smaller one to route to the output. TODO: Either re-seperate using a smaller chip with bigger ram (ideal). Or just get rid of smaller one together.
