# Thymio3 module for MicroPython

import thymio
import uasyncio as asyncio

class ThymioEvents:
    def __init__(self):
        self._sound = thymio.SOUND()
        self._recendf = False
        self._recenda = ()
        self._playendf = False
        self._playenda = ()        
        self._run = asyncio.create_task(self._go())  # Thread runs forever
        
    async def _go(self):
        while True:            
            # check sensors status

            if self._sound.record_completed():
                if self._recendf:
                    self._recendf(self._recenda)

            if self._sound.play_completed():
                if self._playendf:
                    self._playendf(self._playenda)

            await asyncio.sleep_ms(50) # 20 Hz

    def record_complete_func(self, func=False, args=()):
        self._recendf = func
        self._recenda = args

    def play_complete_func(self, func=False, args=()):
        self._playendf = func
        self._playenda = args