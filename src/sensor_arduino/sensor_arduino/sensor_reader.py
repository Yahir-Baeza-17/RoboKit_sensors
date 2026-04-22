import rclpy  #Libreria que se encarga de manejar python en ROS
from rclpy.node import Node #Clase que se encarga de crear cualquier " Nodo"
from std_msgs.msg import Int32MultiArray #Clase que nos permite utilizar el tipo de mensaje que se trasmitira (un arreglo numerico)
import serial #Libreria para realizar la comunicación serial con el arduino

class SensorReader(Node):
    #Se definen los parametros del nodo 
    def __init__(self):
        super().__init__('sensor_reader')   # Tópico donde se publicaran las distancias "Canal de transmisión"
        #Declaramos que el nodo sera publicador mandando el tipo de mensaje o dato, el nombre del topico por donde se trasnmitira y los datos de memoria en cola que se guarden
        self.publisher_ = self.create_publisher(Int32MultiArray, 'sensor_distances', 10)
        
        # Configuración Serial (configuramos el puerto correcto y los baudios correctos)
        try:
            #Configuramos la comunicación serial seleccionando el nombre del puerto, la velocidad baudal y el tiempo de espera a recibir los datos 
            self.ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=1)
            #Imprimimos el mensaje de conexion exitosa en la terminal
            self.get_logger().info('Conexion Serial establecida en /dev/ttyACM0')
        except Exception as e:
            #Imprimimos el mensaje de conexión serial erronea 
            self.get_logger().error(f'Error en conexion serial: {e}')

        # Timer para ejecutar la lectura cada 0.1 segundos (10Hz)
        self.timer = self.create_timer(0.1, self.timer_callback)

    def timer_callback(self):
        if self.ser.in_waiting > 0: #Leeos el buffer in_waiting es un contador que nos dice cuantos bytes hay
            line = self.ser.readline().decode('utf-8').rstrip() #Se lee la linea completa hasta un \n,
            #los bytes se decodifican en caracteres, rstrip borra cualquier espacio en blanco o salto de linea que contenga la cadena
            
            # Verificamos que la cadena comience con la letra "D:"
            if line.startswith("D:"):
                try:
                    
                    data_str = line[2:].split(',') #Comenzamos a cortar la cadena a partir de su posición 2, la funcion split separa la cadena cada que encuentra una coma
                    distances = [int(d) for d in data_str] #Transforma los datos de la string data_str en numeros enteros y los guarda en un arreglo llamado distances
                    
                    # Creamos el mensaje de ROS 2
                    msg = Int32MultiArray() #Declaramos que el tipo de mensaje sera Int32MultiArray
                    # Se le asigna el mensaje dentro de .data (seccion de datos)
                    msg.data = distances
                    
                    # Le decimos al publicador que tome el mensaje y lo mande al topico sensor_distances
                    self.publisher_.publish(msg)
                    # Imprimimos las distancias en la termial de Ubuntu
                    self.get_logger().info(f'Distancias: {distances}')
                except ValueError:
                    # Manda mensaje de error por si algun dato no es numerico y llega al nodo
                    self.get_logger().warn('Error al procesar datos seriales')

def main(args=None):
    rclpy.init(args=args) #inicializa ROS 2
    sensor_node = SensorReader() #Se crea el objeto SensorReader para crear el nodo 
    rclpy.spin(sensor_node) #Mantiene el el nodo en un bucle infinito para que no se cierre
    #Se cierra el nodo y se limpian los datos
    sensor_node.destroy_node() 
    rclpy.shutdown()
# Condicionamos para ejecutar el archivo desde la linea de comando, entonces nos mandará a la función main
if __name__ == '__main__':
    main()