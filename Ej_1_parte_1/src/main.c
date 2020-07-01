#include "driver.h"
#include "FreeRTOSConfig.h"
#include "userTasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"




int main( void )
{

    boardConfig();	 // Inicializar la placa

    //Inicializacion del driver de UART

    init_tasks();

    // gestion de errores de los semaforos y mutex


    vTaskStartScheduler();


    return 0;
}
