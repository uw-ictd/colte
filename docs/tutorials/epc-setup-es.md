---
title: 'Configuración de la EPC "Red en una caja" '
parent: Tutoriales
layout: page
---

# EPC Instalación y Configuración

El paquete CoLTE puede ejecutarse en las versiones de 64 bits de Ubuntu 18.04 LTS (Bionic
Beaver), Ubuntu 20.04 (Focal Fossa) o Debian 10 (Buster). Este tutorial
asume que está utilizando una instalación nueva de Ubuntu 20.04 en un sistema basado en x86-64
mini PC.

Nota: cuando esté instalando Ubuntu, debe elegir la opción "instalación básica"
que no instala software adicional innecesario. En instalaciones anteriores, esto ha llevado
a los conflictos de versión.

## I. Arquitectura LTE

![Diagram of LTE architecture including 4 main sections: User equipment (UE), eNodeB base station, Evolved Packet Core (EPC), Upstream IP networks/Internet](https://i.imgur.com/dMZQVDl.png)

CoLTE simplifica la implementación y configuración de los elementos Evolved Packet Core (EPC) de una red LTE utilizando el paquete Open5GS. El EPC proporciona funciones de software básicas, como la gestión de suscriptores y el enrutamiento del tráfico de usuarios a Internet. Se conecta a la "estación base" de radio, llamada eNodeB (eNB), que luego habla con el Equipo de usuario (UE), es decir, su teléfono celular o dispositivo de acceso.


Para empezar, el componente más importante que debe conocer en el EPC es el 'MME', que administra el proceso del eNB y cualquier dispositivo de usuario final que se conecte a la red (puede pensar en esto como 'iniciar sesión') para que pueden comenzar a enviar datos. En el caso de los usuarios, el MME debe solicitar al componente de software HSS (esencialmente una base de datos de usuarios) las credenciales (claves secretas compartidas únicas para cada usuario) para verificar que una tarjeta SIM determinada puede unirse a la red. El MME es el componente de software cuyos registros de salida debe verificar primero en busca de mensajes de error si algo va mal con la red.

(Puede encontrar documentación y diagramas más detallados de la arquitectura del software Open5GS en la página Open5GS [Inicio Rápido](https://open5gs.org/open5gs/docs/guide/01-quickstart/). Su software es compatible con 4G y 5G, y solo necesita ejecutar un subconjunto de los componentes de software para 4G. Para su referencia, a partir de agosto de 2021, estos componentes de software son: y en Ubuntu se ejecutan como servicios de [systemd](http://manpages.ubuntu.com/manpages/bionic/man1/systemd.1.html)).

## II. CoLTE Installation

Ensure all Ubuntu packages are up-to-date:

```bash
sudo apt update && sudo apt full-upgrade
```
El epc CoLTE se puede instalar desde nuestro repositorio o compilar desde el código fuente. Para este tutorial,
vamos a instalar desde el repositorio para asegurarnos de obtener una versión
reciente del software y acceder a las actualizaciones a través de apt.

```bash
echo "deb [signed-by=/usr/share/keyrings/colte-archive-keyring.gpg] http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /usr/share/keyrings/colte-archive-keyring.gpg http://colte.cs.washington.edu/colte-archive-keyring.gpg
sudo apt install software-properties-common
sudo add-apt-repository ppa:open5gs/latest
sudo apt update
sudo apt install colte-cn-4g
```

## III.Configuración de Interfaces de Red

### A. Configuración Recomendada


Para esta configuración recomendada, **requerimos una máquina EPC con 2 o más puertos ethernet** (_en nuestro caso_, las interfaces ethernet correspondientes a estos puertos se denominan enp1s0 y enp4s0). El puerto ethernet denominado 'enp1s0' se utiliza como puerto [WAN](https://en.wikipedia.org/wiki/Wide_area_network), que accede a las redes ascendentes y, finalmente, a Internet. Está conectado físicamente a través de un cable ethernet a un enrutador que puede darle acceso a Internet (por ejemplo, el enrutador de nuestro ISP). El llamado 'enp4s0' se conectará a nuestra red LTE privada y está conectado físicamente a través de un cable ethernet a la radio eNB. (Nuestro modelo mini-P tiene 4 puertos ethernet).

Para ingresar los valores apropiados _en su caso_, deberá averiguar los nombres de las interfaces de ethernet de su computadora. Use el comando `ip a` en la línea de comando. Aparecerá una lista de interfaces de red en el terminal. Encuentre los correspondientes a sus puertos ethernet (sus nombres generalmente comienzan con "eth", "enp", o "enx").

Para Ubuntu 20.04, actualmente estamos usando el programa Netplan para administrar nuestra configuración de red.
Cree un archivo en el directorio `/etc/netplan` (es decir, una carpeta) llamado `99-colte-config.yaml` y agregue las siguientes líneas, sustituyendo los nombres de interfaz y subredes correctos para su configuración:


```yaml
# CoLTE network configuration
network:
  ethernets:
    enp1s0: # name of interface used for upstream network
      dhcp4: yes
    enp4s0: # name of interface going to the eNB
      dhcp4: no
      addresses:
        - 192.168.150.2/24 # list all downstream networks
        - 192.168.151.2/24
  version: 2
```
Nota: Netplan aplicará los archivos de configuración en este directorio en el orden numérico del prefijo del nombre de archivo (es decir, 00-\*, 01-\*, etc.). Cualquier interfaz configurada en un archivo anterior se sobrescribirá con archivos de configuración con números más altos, por lo que creamos un archivo con el prefijo 99-\* para reemplazar todos los demás archivos de configuración.


**Explicación rápida:**
Para obtener conectividad a Internet para el EPC, configuramos la interfaz de ethernet "upstream" o "WAN" (enp1s0) para solicitar una dirección IP a través de [DHCP](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol) de un enrutador ascendente al que está conectado (como suele hacer su computadora cuando la conecta a un enrutador doméstico típico), que pasa su tráfico hacia y desde Internet global. Es por eso que tenemos la línea `dhcp4: yes` debajo de nuestro nombre de interfaz `enp1s0`. No necesitamos esta interfaz para tener ninguna otra dirección IP.

A la interfaz ethernet "descendente" (enp4s0) conectada al eNB se le asignan dos direcciones IP y subredes, que se configuran estáticamente (_no_ por DHCP, de ahí el `dhcp4: no`). En nuestro caso, necesitamos esta interfaz para hablar con el Baicells Nova 233 eNB que usamos. Nuestro eNB tiene la dirección IP local (LAN) predeterminada de `192.168.150.1`. También debemos configurar su dirección WAN (por cualquier motivo, debe ser diferente) en `192.168.151.1`, como en [Este tutorial de configuración de eNB](https://docs.seattlecommunitynetwork.org/infrastructure/sas-setup.html). Es por eso que tenemos la sección `addresses:` que establece las direcciones IP estáticas del EPC en `192.168.150.2/24` y `192.168.151.2/24`. Dado que estas direcciones IP están en la misma subred que las direcciones IP eNB, podrán comunicarse entre sí automáticamente, _sin un enrutador en el medio_, lo que ayudará a enrutar los paquetes de comunicaciones entre las dos direcciones.

A continuación, también proporcionamos una configuración alternativa en caso de que aún no tenga una máquina con 2 puertos ethernet o un dongle adaptador de USB a ethernet. Sin embargo, solo se recomienda la primera configuración para implementaciones por motivos de seguridad.
**La alternativa solo debe de usarse para pruebas**.

### B. NO recomendado para despliegues

Si aún no tiene una máquina con 2 puertos ethernet o un dongle adaptador de USB a ethernet, puede usar temporalmente una máquina con un solo puerto ethernet junto con un simple conmutador o enrutador. Si usa un conmutador simple, puede seguir las mismas instrucciones, pero conecte los tres EPC, eNB y el enrutador de Internet ascendente al conmutador. Si usa un enrutador, es posible que deba configurar el enrutador para asignar 2 direcciones IP estáticas privadas a cada uno de los EPC (es decir,
`192.168.150.2`, `192.168.151.2`) y  eNB (i.e. `192.168.150.1`,
`192.168.151.1`), de tal manera que NAT correctamente el tráfico ascendente y también enrute el tráfico local entre el EPC y el eNB

```yaml
# Network config EPC with single ethernet card
# A switch is used to connect all devices
network:
  ethernets:
    enp1s0: # name of ethernet interface
      dhcp4: true
      addresses:
        - 192.168.150.2/24 # list all downstream networks
        - 192.168.151.2/24
  version: 2
```

Una vez que se haya modificado este archivo (o la configuración de su enrutador), reinicie el demonio de red para aplicar los cambios de configuración:

```bash
sudo netplan try
sudo netplan apply
```
Si el eNB se conectará a su propio puerto ethernet EPC dedicado, como en la configuración recomendada anteriormente, es posible que deba conectar ese puerto ethernet EPC a algo (por ejemplo, el eNB, un interruptor, otra máquina) a través de un cable ethernet para reactivar el interfase arriba (para que se active y tome las direcciones IP asignadas). Esto se debe a que open5gs MME necesita "bind/vincular" (o asociar) su interfaz S1 a una de esas direcciones IP (en este caso, `192.168.0.2`). Hasta que esas direcciones IP existan en su máquina, el MME arrojará errores continuamente si intenta ejecutarlo.



## IV.Configuración CoLTE

### A. Utilizando `colteconf`
CoLTE simplifica la configuración de la red LTE al consolidar los archivos de configuración relevantes en el directorio `/etc/colte`. El archivo de configuración principal es `/etc/colte/config.yml`. Actualice este archivo de la siguiente manera:

```yaml
# REMEMBER TO CALL "sudo colteconf update" AFTER CHANGING THIS FILE!

# basic network settings
enb_iface_addr: 192.168.150.2 # local IP for eNB interface
wan_iface: enp1s0 # ethernet WAN (upstream) interface name
network_name: YourNetworkName
lte_subnet: 10.45.0.0/16 # End User subnet

# PLMN = first 5 digits of IMSI = MCC+MNC
mcc: 910
mnc: 54

# advanced EPC settings
dns: 8.8.8.8

# database connection settings (for Haulage + WebGui + WebAdmin)
mysql_user: haulage_db
mysql_password: haulage_db
mysql_db: haulage_db

# use these vars to turn services ON (also starts at boot) or OFF
metered: true
nat: true
epc: true
```

**Explicación rápida:**
`enb_iface_addr` se refiere a la dirección IP de la interfaz ethernet conectada al eNB (que configuramos en la sección III anterior). `wan_iface` se refiere al _nombre_ de la interfaz WAN ethernet conectada a la fuente de Internet lo descubrimos en la sección III anterior). `network_name` es un nombre personalizable que podemos configurar para identificar su red LTE (¡le agrega algo de sabor!) `lte_subnet` se refiere a las direcciones IP locales/privadas que la red le dará a los dispositivos de los usuarios internamente (no tienes que preocuparte por esto). `#PLMN` se refiere a la [Red móvil pública terrestre](https://en.wikipedia.org/wiki/Public_land_mobile_network), en la que nuestra red debe tener una identificación de operador única definida por el "código de país móvil (MCC)" y "código de red móvil (MNC)". Hemos utilizado números arbitrarios no asignados por ahora. `dns` se refiere a la dirección IP del [Sistema de nombres de dominio](https://developers.google.com/speed/public-dns) servidor que usará el EPC, con el valor predeterminado establecido en el servidor público de Google en 8.8.8.8. `# database connection settings` son parámetros internos que se usan para acceder a las bases de datos de información del usuario; se romperán si los cambia. `metered: true` significa que el sistema realizará un seguimiento predeterminado de la cantidad de bits utilizados por cada usuario, así como también ejecutará un panel de administración de usuarios que asume el uso "prepago", si está instalado con el paquete `colte-prepaid`.


Una vez que el archivo haya sido editado a su gusto, ejecute:

```bash
sudo colteconf update
```
Esto actualizará la configuración y reiniciará los servicios.

### B. Conectando el eNB a Internet

Tenga en cuenta que la herramienta `colteconf` tal como está escrita actualmente solo configura la traducción de direcciones de red ([NAT](https://en.wikipedia.org/wiki/Network_address_translation)) para lo que hemos llamado "Subred LTE", que en nuestro anterior la configuración de ejemplo es `10.45.0.0/16`. Sin embargo, como se explicó anteriormente, el eNB actualmente tiene las direcciones IP `192.168.150.1` y `192.168.151.1`-- _[direcciones IP privadas](https://en.wikipedia.org/wiki/Private_network)_ que no pueden utilizarse en la Internet pública. Por lo tanto, para enrutar con éxito el tráfico de red del eNB a Internet, debemos agregar una regla de enrutamiento en la computadora del EPC que realice NAT, lo que permite que los paquetes de la subred del eNB salgan del puerto WAN del EPC _enmascarados como_ provenientes de la dirección IP del EPC. a la red aguas arriba.

Puede haber una manera más fácil de hacer esto, pero hasta ahora hemos encontrado la forma más limpia y confiable de usar la herramienta de línea de comandos `iptables`. En la Terminal del EPC, ejecute el siguiente comando para agregar una regla NAT para la subred del eNB:


```bash
sudo iptables -t nat -A POSTROUTING -s 192.168.151.0/24 -j MASQUERADE
```

**Explicación rápida:** La opción `-t nat` le dice a IPTables que instale la regla en la "tabla" correcta que contiene todas las reglas NAT, y la opción `-A` significa que estamos **A**dding (Agregando) la regla en lugar de **D**eleting (Eliminando) (`-D`). `POSROUTING` es la "cadena", o lista particular de reglas, en la que debe ir este tipo de regla NAT (más información [aquí](https://rlworkman.net/howtos/iptables/chunkyhtml/c962.html) y en este [diagrama](https://upload.wikimedia.org/wikipedia/commons/3/37/Netfilter-packet-flow.svg) si está interesado). `-s 192.168.151.0/24` significa que estamos aplicando esta regla a los paquetes de las direcciones IP de **fuente** descritas por la subred `192.168.151.0/24`. `-j MASQUERADE` significa que la acción a la que **Saltaremos** como resultado de esta regla es "enmascarar" la dirección IP de origen como la dirección IP WAN de mi EPC.


### C. Monitoreando los servicios de los software CoLTE y Open5GS

Los servicios de registro y monitoreo incorporados de Ubuntu se pueden usar para monitorear los servicios de red centrales. Por ejemplo, para ver los registros de salida del componente de software MME que describimos en la primera sección, ejecute el siguiente comando en la Terminal:

```bash
sudo journalctl -f -u open5gs-mmed.service
```

O

```bash
sudo systemctl status open5gs-mmed.service
```

_La tecla Tab  puede ayudar a completar el nombre del servicio para systemctl_

¡Aprender a leer los registros de salida es realmente importante para administrar la infraestructura de software! Simplemente buscar en Google los mensajes de salida que parecen importantes pero que no comprende puede ser un buen primer paso para descubrir cómo funciona un sistema. Otra herramienta interesante para investigar es [Wireshark](https://www.wireshark.org/), que es esencialmente una versión de interfaz gráfica de usuario (GUI) de [tcpdump](https://www.tcpdump.org/) herramienta de línea de comandos que puede mostrarle los [paquetes](https://en.wikipedia.org/wiki/Network_packet) de comunicaciones que fluyen a través de las diversas tarjetas de red en su computadora.

Aquí hay algunos comandos más útiles para administrar los servicios de systemd, que se pueden usar para iniciar, detener y volver a cargar los componentes del software después de haber cambiado su configuración o si se han producido errores y deben reiniciarse:

```bash
sudo systemctl start open5gs-mmed.service
sudo systemctl stop open5gs-mmed.service
sudo systemctl restart open5gs-mmed.service
```

## V. Configuración 'Persistente' de IPTables de  CoLTE
Como se mencionó anteriormente, CoLTE configura las reglas de IPTables para asegurarse de que los paquetes se enruten correctamente dentro de
el EPC. Las reglas de IPTables deben ser persistentes en los reinicios con el
Paquete `iptables-persistent`:

```bash
sudo apt install iptables-persistent
```

La instalación de este paquete guardará las reglas actuales de iptables en su archivo de configuración, `/etc/iptables/rules.v4`.

Nota: `iptables-persistent` lee el contenido de este archivo al arrancar y aplica todas las reglas de iptables que contiene. Si necesita actualizar las reglas o volver a aplicar manualmente, puede usar los siguientes comandos. Esto no debería ser necesario en circunstancias normales:

```bash
sudo iptables-save > /etc/iptables/rules.v4
sudo iptables-restore < /etc/iptables/rules.v4
```

## VI. Administración y Gestión de Usuarios

### A. Utilizando la Linea de Comandos `coltedb`

CoLTE viene con el comando `coltedb` que se puede usar para modificar la base de datos del usuario a través de la línea de comandos. Ejecute `coltedb` sin ningún argumento para ver una lista de los comandos disponibles.

Para agregar un nuevo usuario con una tarjeta SIM determinada, necesitará varios datos para cada tarjeta SIM. El fabricante de la tarjeta SIM debe poner a su disposición estos valores como una hoja de cálculo o un archivo de texto cuando los compre.
**¡¡POR FAVOR, MANTENGA ESTA INFORMACIÓN EN SECRETO!** Esto es esencial para la privacidad y seguridad de su red.

- IMSI
   - identificador único para la tarjeta SIM
   - Proporcionado por el fabricante
- MSISDN
   - un número arbitrario que representa el "número de teléfono" del usuario
   - podrían ser los últimos 5 o más dígitos del IMSI - inventa esto si no se le proporcionado.
- Dirección IP
   - este valor establece una IP estática privada para cada tarjeta SIM
   - también eres libre de configurar esto
- Llave
   - clave privada del usuario utilizada en el cifrado LTE
   - el fabricante proporciona
-OPC
   - clave privada "portadora" utilizada en el cifrado LTE
   - el fabricante proporciona
- APN (_opcional)_
   - Nombre del punto de acceso
   - para algunos modelos de teléfonos CBRS LTE como el LG G8 ThinQ, el APN enviado por  el teléfono está codificado para ser la cadena "ims", por lo que la única solución que hemos
     encontrado es configurar el APN en el EPC para que coincida.

Para agregar un solo usuario nuevo en la línea de comando, use el siguiente formato de comando:


```bash
sudo coltedb add imsi msisdn ip key opc [apn]
```

Por ejemplo, una línea con algunos valores ficticios insertados podría verse así (sin APN):

```bash
sudo coltedb add 460660003400030 30 192.168.151.30 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F
```

### B. Adición masiva mediante un script

El script de shell "bulk_add.sh" se proporciona para su comodidad en la carpeta [conf/](https://github.com/uw-ictd/colte/tree/main/conf) del repositorio de github. Toma un solo argumento, el nombre de archivo (ruta completa si no está en el mismo directorio) de un archivo (digamos user_sims.txt) que contiene la información de la tarjeta SIM de múltiples usuarios, uno por línea.

Aquí hay un ejemplo de 3 líneas de un archivo user_sims.txt (con información de SIM ficticia y el conjunto de APN para cada usuario):

```
460660003400032 32 192.168.151.32 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
460660003400033 33 192.168.151.33 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
460660003400034 34 192.168.151.34 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
```

Luego, para agregarlos todos a la vez a la base de datos, ejecutaría:

```bash
sudo bulk_add.sh user_sims.txt
```
