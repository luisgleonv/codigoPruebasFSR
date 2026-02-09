#include <hal/nrf_saadc.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);

  nrf_saadc_enable();

  // CONFIGURACIÓN CRUCIAL DEL CANAL
  nrf_saadc_channel_config_t config = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain       = NRF_SAADC_GAIN1_3,           // de 1/3, es decir multiplica por 3 (se divide)
    .reference  = NRF_SAADC_REFERENCE_INTERNAL, // Referencia interna de 0.6V
    .acq_time   = NRF_SAADC_ACQTIME_15US,      // Tiempo suficiente para potenciómetro de 10k
    .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst      = NRF_SAADC_BURST_DISABLED
  };

  // Inicializar Canal 0 apuntando al pin AIN0 (Pin A0 de la Nicla)
  nrf_saadc_channel_init(0, &config);
  nrf_saadc_channel_input_set(0, NRF_SAADC_INPUT_AIN0, NRF_SAADC_INPUT_DISABLED);
  
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_12BIT); // Definimos 12 bits explícitamente
}

void loop() {
  nrf_saadc_value_t val;
  float voltaje= 0;
  nrf_saadc_buffer_init(&val, 1);

  nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
  nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);

  while (!nrf_saadc_event_check(NRF_SAADC_EVENT_END));
  nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

  Serial.print("Voltaje: ");
  voltaje = val*1.8/4095;
  Serial.print(voltaje, 3); //Tres decimales significativos
  Serial.println(" [V]");
  

  delay(250);
}