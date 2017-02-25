# init.S
# Low level initialization of microcontroller
# Rasmus Kallqvist, 24 January 2017

.align 2 
.text

# name   : ENABLE_INTERRUPT
# brief  : enable interrupts globally 
# author : Rasmus Kallqvist, 14 February 2017
.global enable_interrupt
enable_interrupt:
  ei                        # enable interrupts globally 
  jr    $ra                 # return
  nop                       # delay slot filler
