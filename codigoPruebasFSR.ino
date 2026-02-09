#include <hal/nrf_saadc.h>


float voltaje_SAADC = 0; //voltaje que devuelve el SAADC

void setup() {
  //Comunicación serial para diagnóstico rápido de lectura del ADC
  Serial.begin(115200);
  while(!Serial);

  //Función con configuración del SAADC
  configuracion_SAADC();
  
}

void loop() {
  voltaje_SAADC = lectura_SAADC();
  Serial.print("Voltaje: ");
  Serial.print(voltaje_SAADC, 3); //Tres decimales significativos
  Serial.println(" [V]");
  

  delay(250);
}

void configuracion_SAADC (void){
  //Realizado con acceso directo a registros debido a que la API de Analog era muy limitada, necesitaba configurar más
  //Muchas de las configuraciones se obtuvieron de AI, necesario conseguir las bibliotecas en donde tiene guardado todo y estudiarlas, y en caso de mejora realizarlo.
  
  nrf_saadc_enable(); //Habilita el SAADC de la Nicla (NRF porque es la familia del SoC embebido)

  // CONFIGURACIÓN CRUCIAL DEL CANAL
  nrf_saadc_channel_config_t config = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain       = NRF_SAADC_GAIN1_3,           // de 1/3, es decir multiplica por 3 (se divide)
    .reference  = NRF_SAADC_REFERENCE_INTERNAL, // Referencia interna de 0.6V
    .acq_time   = NRF_SAADC_ACQTIME_15US,      // Tiempo de adquisición de 15 us, estudiar si es mucho o poco tiempo
    .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst      = NRF_SAADC_BURST_DISABLED
  };

  // Inicializar Canal 0 apuntando al pin AIN0 (Pin A0 de la Nicla, físicamente es el J1-8, el que dice ADC1 en el pin-out)
  nrf_saadc_channel_init(0, &config);
  nrf_saadc_channel_input_set(0, NRF_SAADC_INPUT_AIN0, NRF_SAADC_INPUT_DISABLED);
  
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_12BIT); // Definimos 12 bits explícitamente de resolución
}

float lectura_SAADC(void){
nrf_saadc_value_t val;
  nrf_saadc_buffer_init(&val, 1);

  nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
  nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);

  while (!nrf_saadc_event_check(NRF_SAADC_EVENT_END));
  nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

  return val*1.8/4095; //Multiplicar entre el V_ref (explicado que es la referencia interna de 0.6 multiplicada por 3) y el tamaño de palabra menos uno (2**12-1)
}