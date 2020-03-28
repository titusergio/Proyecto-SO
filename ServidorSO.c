#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>




int IntroducirDatosMySQL(char nombre[20], char contra[20], char email[20]){
	MYSQL *conn;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int err;
	char consulta[80];

	
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//inicializar la conexi??n, entrando nuestras claves de acceso y
	//el nombre de la base de datos a la que queremos acceder 
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "juego",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	if(strcmp(email, "0")=="0"){//volem fer consulta
		/*if(err!=0){
			printf("Error al consultar base de datos");
			exit(1);
		}*/
	}else{//ens volem registrar
		strcpy(consulta, "SELECT jugador.usuario FROM jugador, partida, resumen WHERE(jugador.");
		strcat(consulta, email);
		strcat(consulta,"');");
		err=mysql_query(conn, consulta);
		if(err!=0){
			printf("Error al consultar base de datos");
			exit(1);
		}
		resultado = mysql_store_result (conn);
		row = mysql_fetch_row(resultado);
		if(row==NULL){
			strcpy(consulta, "INSERT INTO jugador VALUES('");
			strcat(consulta, nombre);
			strcat(consulta,"', '");
			strcat(consulta, contra);
			strcat(consulta,"', '");
			strcat(consulta, email);
			strcat(consulta,"');");
			err = mysql_query(conn, consulta);
			if (err!=0) {
				printf ("Error al introducir datos en la base %u %s\n", 
						mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
		}else{
			return 1;//ja existeix
		}
	}
	
	mysql_close (conn);
	return 0;


}



int AtenderCliente (char peticion[50]){
	
	char nombre[20];
	char contra[20];
	char email[20];
	char consulta[100];
	int resultado;
	
	char *p = strtok( peticion, "/");
	int codigo =  atoi (p);
	p = strtok(NULL, '/');
	strcpy(nombre, p);
	p = strtok(NULL, '/');
	strcpy(contra, p);


	//ENS Volem registrar
	if(strcmp(nombre, "\0")==0||strcmp(contra, "\0")==0){
		resultado=0;
	}else{
		if(codigo==1){
			p = strtok(NULL, '/');
			strcpy(email, p);
			if(strcmp(email, "\0")==0){
				resultado=0;
			}else{
				resultado = IntroducirDatosMySQL(nombre, contra, email);
			}
		}else{
			resultado = IntroducirDatosMySQL(nombre, contra, "0");
		}
	}
	return resultado;
	
	/*if (codigo ==1){ //quieren registrarse
		p = strtok( NULL, "/");
		strcpy (nombre, p);
		p = strtok(NULL, "/");
		strcpy(contra, p);
		p = strtok(NULL, "/");
		strcpy(email, p);
		printf("Nombre usuario: %s, Contraseña: %s, Email: %s", nombre, contra, email);
		//creem la consulta amb la base de dades
		strcpy(consulta, "SELECT username, email FROM jugador WHERE username='");
		strcat(consulta, nombre);
		strcat(consulta,"' AND email='");
		strcat(consulta,email);
		strcat(consulta, "';");
		
		resultado=DatosMySQL(consulta, codigo);
		return resultado;//resultado pot ser 0-> si ja existeix l'usuari, 1->S'ha registrat
		
	}else if(codigo==2){//quieren entrar
		p = strtok( NULL, "/");
		strcpy (nombre, p);
		p = strtok(NULL, "/");
		strcpy(contra, p);
		printf("Nombre usuario: %s, Contraseña: %s", nombre, contra);
		
		strcpy(consulta, "SELECT contrasena FROM judador WHERE username='");
		strcat(consulta, nombre);
		strcat(consulta,"';");
		
		resultado=DatosMySQL(consulta, codigo);
		return resultado;//resultado pot ser 0->si tot ha anat bé, 1->si l'usuari ja existeix, 2-> si l'usuari o la contrasenya estan malament
		
	}else{//consultas
		
	}*/
	
}




int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char buff[512];
	char buff2[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	int i;
	// Atenderemos solo 5 peticione
	for(i=0;i<7;i++){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		// Ahora recibimos su nombre, que dejamos en buff
		ret=read(sock_conn,buff, sizeof(buff));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		//Escribimos el nombre en la consola
		
		printf ("Se ha conectado: %s\n",buff);
		
		int res = AtenderCliente(buff);
		printf("\nResultado: %d\n", res);
/*		
		char contra[20];
		char email[20];
		char *p = strtok( buff, "/");
		int codigo =  atoi (p);
		p = strtok( NULL, "/");
		char nombre[20];
		strcpy (nombre, p);
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		
		if (codigo ==1){ //quieren registrarse
			p = strtok(NULL, "/");
			strcpy(contra, p);
			p = strtok(NULL, "/");
			strcpy(email, p);
			printf("Nombre usuario: %s, Contraseña: %s, Email: %s", nombre, contra, email);
			
		}else if(codigo==2){//quieren entrar
		
		}else{//consultas base de datos
			
		}
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (buff2,"SI,");
			else
				strcpy (buff2,"NO,");
		
	*/
		printf ("%s\n", buff2);
		// Y lo enviamos
		write (sock_conn,buff2, strlen(buff2));
		
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
