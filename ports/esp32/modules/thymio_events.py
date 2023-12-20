# Thymio3 module for MicroPython

import thymio
import uasyncio as asyncio

def onevent(event_type):
    def decorator(func):
        ThymioEvents._event_handlers[event_type] = func
        #print(ThymioEvents._event_handlers)
        return func
    return decorator

class ThymioEvents:
    EVENTS = ["PLAY_COMPLETED", "RECORD_COMPLETED"]
    _event_handlers = {}

    def __init__(self):
        self._sound = thymio.SOUND()

        for event_type in self.EVENTS:
            self._event_handlers[event_type] = None
        self._run = asyncio.create_task(self._go())  # Thread runs forever
        
    async def _go(self):
        while True:            
            # check sensors status

            if self._sound.play_completed():
                if self._event_handlers["PLAY_COMPLETED"] is not None:
                    self._event_handlers["PLAY_COMPLETED"]()

            if self._sound.record_completed():
                if self._event_handlers["RECORD_COMPLETED"] is not None:
                    self._event_handlers["RECORD_COMPLETED"]()

            await asyncio.sleep_ms(50) # 20 Hz


