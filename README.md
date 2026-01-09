
本例使用 Ardiono UNO 控制 Bluetooth HC05 Module 

# 接線 

Pin #5: I: SoftSerial RX <----> HC5#TX
Pin #6: O: SoftSerial TX <--(電阻分壓降壓)--> HC5#RX
Pin #7: I: <----> HC5#Status <--(並聯)--> LED+電阻
Pin #8: O: <----> HC5#EN
Pin #4: O: <----> Relay <----> HC#PWR(5V)   --x 直接推,每1.4s 會掉一次電壓(2.5v) <-- 不斷Reset #GND...
