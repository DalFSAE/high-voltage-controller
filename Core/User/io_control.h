 #ifndef IO_CONTROL_H
 #define IO_CONTROL_H

void enable_precharge_relay();
void enable_air_negative();
void enable_air_positive();

void disable_precharge_relay();
void disable_air_negative();
void disable_air_positive();

void disable_all_relays();

#endif