# 🔑 Synchronization Primitives in FreeRTOS
## [1. Binary Semaphore](/Semaphore/)

[![Youtube Video](https://img.youtube.com/vi/cdcLoYO5OAY/0.jpg)](https://www.youtube.com/watch?v=cdcLoYO5OAY) 

* Works like an ON/OFF flag.
* Commonly used for` task-to-task` or `ISR-to-task` signaling.
* Example: An ISR “gives” the semaphore when data is ready, and a task “takes” it to
process the data.
## [2. Counting Semaphore](/Semaphore/)

[![Youtube Video](https://img.youtube.com/vi/HgZR4UvT7tY/0.jpg)](https://www.youtube.com/watch?v=HgZR4UvT7tY) 

* Similar to binary, but the semaphore value can be greater than 1.
* Useful for `counting event`s or managing access to a pool of identical resources.
* Example: Controlling access to a pool of 5 buffers.
## [3. Mutex  & Recursive Mutex](/Mutex/)

[![Youtube Video](https://img.youtube.com/vi/DO_xbIu7GK0/0.jpg)](https://www.youtube.com/watch?v=DO_xbIu7GK0) 

### Mutex (Mutual Exclusion Semaphore)
* Protects a `shared resource` (like UART, I2C, or a global variable) so only one task
uses it at a time.
* Has a `priority inheritance mechanism` to avoid priority inversion.

### Recursive Mutex
* A special kind of mutex that allows the same task to `take` the mutex multiple times
(as long as it gives it back the same number of times).
* Useful in cases where a function that takes a mutex is called by another function that
already holds it.

## [5. Event Groups](/EventGroups/)

[![Youtube Video](https://img.youtube.com/vi/BsUXqmotvlE/0.jpg)](https://www.youtube.com/watch?v=BsUXqmotvlE) 

* A collection of bits that tasks can `wait on`.
* Tasks can wait for one or more bits to be set before continuing.
* Useful for synchronization of multiple events.
* Example: A task waits until both “WIFI_CONNECTED” and “SENSOR_READY” flags
are set.
## [6. Task Notifications (Direct-to-Task Notifications)](/Direct_to_Task_Notifications/)

[![Youtube Video](https://img.youtube.com/vi/PYBfxhxOfsg/0.jpg)](https://www.youtube.com/watch?v=PYBfxhxOfsg) 

* Lightweight and faster than semaphores.
* Each task has a built-in `notification value` (like a private semaphore or mailbox).
* Tasks can block waiting for a notification, and ISRs or tasks can send notifications.
* Often recommended instead of binary semaphores when possible.
## [7. Message Queues](/Queue/)

[![Youtube Video](https://img.youtube.com/vi/TAY9shuSdlE/0.jpg)](https://www.youtube.com/watch?v=TAY9shuSdlE) 

* FIFO buffers managed by the kernel.
* Used for `task-to-task` or `ISR-to-task` communication.
* You can send structured data (not just a signal) between tasks.
* Example: A producer task sends sensor readings to a queue,
and a consumer task processes them.
## [8. Stream Buffers](/Stream_Buffer/)

[![Youtube Video](https://img.youtube.com/vi/f9ZCi5Y4fpQ/0.jpg)](https://www.youtube.com/watch?v=f9ZCi5Y4fpQ) 

* One-way communication channel for continuous streams of bytes.
* Designed for sending data from ISR to task (like UART receive)..
* Works like a circular buffer managed by FreeRTOS.
## [9. Message Buffers](/Message_Buffers/)
* Similar to stream buffers, but instead of a continuous stream,
data is sent as discrete messages (each message has a length)..
* Example: Sending packets of varying sizes between tasks.

| Primitive | Purpose | Typical Use Case|
|-----------|---------|-----------------|
Binary Semaphore | Signaling (task/ISR → task) | ISR signals a task |
Counting Semaphore | Counting events / resource pool |Multiple buffers available |
Mutex | Exclusive access to a resource | Protect UART/I2C access
Recursive Mutex | Same task can lock multiple times |Nested function calls
Event Groups | Sync multiple events (bit flags) | Wait for WiFi + Sensor ready
|Task Notifications | Lightweight semaphore/queue alternative | Fast ISR → task signaling
|Message Queue | Send data/messages between tasks | Producer/Consumer
|Stream Buffer | Continuous byte stream | UART/ADC data stream
|Message Buffer | Discrete variable-length messages | Network packets
