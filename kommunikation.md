Umsetzung der Schnittstellen mit MQTT
=====================================

Grundlagen
----------

### Annahmen
* Scheduler kennt die Hostnamen aller knoten.


### Aufbau einer MQTT Nachricht
Eine MQTT Nachricht besteht aus zwei Teilen

* Topic
* Payload in YAML


### Ablauf der Kommunikation
Grundsätzlich ist das ein einfaches Publisher-Subscriber Pattern.
Hier eine grobe Übersicht.

1. Ein Teilnehmer verbinden sich mit einem Server.
  * Ein Teilnehmer melden sich für N Topics an.
  * Der Server schickt alle Nachrichten für die sich ein Teilnehmer angemeldet
    hat.

2. Ein Teilnehmer meldet sich ab.

### Diskussion
* Eine Nachricht pro gestartete VM oder eine Nachricht mit allen gestarteten VMs
  an Scheduler? (siehe Kommentar unter "vm started")
  //J: Macht das einen relevanten Unterschied?
* Einheitlicher result status: "status: success | error" (Eventuell "details:
  <error description>" hinzufügen für mehr Infos.)
  //J: Finde ich gut, sollte in der FASTLib auch eine Klasse dafür geben.
* Einheitlicher task/result Name: "task: <start|stop|migrate> vm", "result: vm
  <started|stopped|migrated>"
  //J: Wichtiger wäre wohl jeder Nachricht eine eindeutige ID zu geben, die beim
       Antworten wieder mitgeschickt wird.

### Online YAML parser
http://yaml-online-parser.appspot.com/


MQTT Topics
-----------

### Struktur
```
fast
+-- migfra
|   +-- <hostname>
|   |   +-- task
|   |   +-- result
|   +-- ...
|   +-- <hostname>
+-- pscom
|   +-- <hostname>
|   |   +-- <procID>
|   |   |   +-- request
|   |   |   +-- response
|   |   |-- ...
|   |   |-- <procID>
|   +-- ...
|   +-- <hostname>
+-- agent
|   +-- <hostname>
|   +-- status
|   +-- task
|   +-- ...
|   +-- <hostname>
+-- application
    +-- <slurm_jobid>
        +-- status
        +-- KPIs
```

### Subscriber
* Scheduler
  * fast/agent/+/status
    Ueberwachung der Statusänderungen *aller* Agenten
  * fast/migfra/+/result
    Ergebnisse von Anfragen des Schedulers an eine der
    Migrationsframework-Instanzen
  * fast/application/+/status
  * fast/application/+/KPIs
* Migration-Framework
  * fast/migfra/<hostname>/task
    Entgegennehmen von Anfragen des Schedulers
  * fast/pscom/<hostname>/<procID>/response
    Antwort vom entsprechenden Prozess, dass alle Verbindungen herunter gefahren wurden
* Agent
  * fast/migfra/<hostname>/result
    Aenderungen im Schedule des betreffenden nodes (z.B. neuer Job)
  * fast/agent/<hostname>/task
    Anfragen des Schedulers an den entsprechenden Agenten (z.B. start/stop
    monitoring)
* pscom
  * fast/pscom/<hostname>/<procID>/request
    Entgegennehmen von Anfragen des Migrationsframeworks
* Anwendungen
  //J: Von mir, aber noch nicht ganz klar ob das so passt
  * fast/migfra/<hostname>/task
    Überwacht um mögliche Migration festzustellen <- so aber keine Möglichkeit einzugreifen
  * fast/application/<hostname>/<pid>/task <- <pid> nicht über VMs hinweg identisch / <hostname>=VM name?
    Entgegennehmen der Anfragen des Agenten / Schedulers / MigFra

### Publisher
* Scheduler
  * fast/migfra/<hostname>/task
    Starten, Stoppen und Migrieren von VMs
  * fast/agent/<hostname>/task
    starten/stoppen des Monitorings; Konfiguration der KPIs
* Migration-Framework
  * fast/migfra/<hostname>/result
    VM gestartet/gestoppt/migriert
  * fast/pscom/<hostname>/<procID>/request
    Herunterfahren von pscom Verbindungen
* Agent
  * fast/agent/<hostname>/status
    Anmeldung des Agenten beim Scheduler
* pscom
  * fast/pscom/<hostname>/<procID>/response
    Information an Migrationsframework, dass Verbindungen herunter gefahren wurden
* Anwendungen
  //J: Von mir, aber noch nicht ganz klar ob das so passt
  //* fast/application/<hostname>/<pid>/status <- <pid> nicht über VMs hinweg identisch / <hostname>=VM name?

  * fast/applicaiton/<slurm_jobid>/status
    Status Informationen der Anwendung
  * fast/application/<slurm_jobid>/KPIs

Nachrichtenformat
-----------------
Im Folgenden werden die Nachrichten, welche über die oben definierten Topics
verteilt werden, definiert. Hierbei handelt es sich jeweils um Nachrichten im
YAML Format. Die unterschiedlichen Nachrichten sind nach ihrer Quelle sortiert.

### Scheduler
#### Agenten Initialisieren
Der Scheduler meldet sich beim Agenten und liefert eine initiale Konfiguration.
* topic: fast/agent/<hostname>/task/init_agent
* Payload
  ```
  task: init agent
  KPI:
    categories:
      - energy consumption: <energy>
      - compute intensity: <high,medium,low>
      - IO intensity: <high,medium,low>
      - communication intensity (network): <high,medium,low>
      - expected runtime: <high,medium,low>
    repeat: <number in seconds how often the KPIs are reported>
  ```
* Erwartetes Verhalten:
  Der Agent merkt sich die gesendete Konfiguration und reagiert
  entsprechend. Der für den Knoten zuständige Scheduler empfängt die
  Nachrichten über das angegebene Topic.
* Antwort: Default result status

#### Monitoring/Tuning beenden
Der Agent soll aufhören die Anwendung zu überwachen.
* topic: fast/agent/<hostname>/task/stop_monitoring
* Payload
  ```
  task: stop monitoring
  job-description:
    job-id: <job id>
    process-id: <process id of the vm>
  ```
* Erwartetes Verhalten:
  Agent hört auf den Prozess zu überwachen.
* Antwort: Default result status

#### VMs starten
Anfrage des Schedulers an die entsprechende Migrationsframework-Instanz
eine oder mehrere VMs zu starten.
Diskussion: VM vorbereiten in Scheduler Skripten oder von Migfra?
* topic: fast/migfra/<hostname>/task
* Payload
  ```
  host: <string>
  task: start vm
  id: <uuid> //J: <- was ist das?
  vm-configurations:
    - name: <string>
      xml: <Pfad zu XML>
      overlay-image: <Pfad zum neuen Overlay-Image>
      base-image: <Pfad zu Basisimage>
    - name: <string>
      xml: <Pfad zu XML>
      overlay-image: <Pfad zum neuen Overlay-Image>
      base-image: <Pfad zu Basisimage>
    - ..
  ```
* Erwartetes Verhalten:
  VMs werden auf dem entsprechenden Host gestartet.
* Antwort: Default result status? Oder doch lieber modifiziert mit eindeutige IDs für die VMs?

#### VM stoppen
Annahme: VM wird gestoppt wenn die Anwendung fertig ist / beendet werden soll.
* topic: fast/migfra/<hostname>/task
* Payload
  ```
  host: <string>
  task: stop vm
  id: <uuid>
  list:
    - name: <vm name>
    - name: <vm name>
    - ..
  ```
* Erwartetes Verhalten:
  VM wird gestoppt.
* Antwort: Default result status

#### Migration starten
Diese Nachricht informiert die zuständige Migrationsframework-Instanz darüber,
dass die Anwendung vom Quellknoten auf den Zielknoten migriert werden soll.
* topic: fast/migfra/<hostname>/task
* Payload
  ```
  host: <string>
  task: migrate vm
  id: <uuid>
  name: <vm name>
  destination: <destination hostname>
  time-measurement: true
  parameter:
    retry-counter: <counter>
    migration-type: live | warm | offline
    rdma-migration: true | false
  ```

* Erwartetes Verhalten:
  VM wird vom Migrationsframework gestartet und anschließend wird eine
  entsprechende Statusinformation über den 'scheduler' channel gechickt.
* Antwort: Default result status

### Migration-Framework
#### VM gestartet
Nachdem die VM gestartet ist und bereit ist eine Anwendung auszuführen
informiert die entsprechende Migrationsframework-Instanz den Schduler darüber.
* topic: fast/migfra/<hostname>/status
* Payload:
  ```
  scheduler: <hostname/global>
  result: vm started
  list:
    - name: <vm-hostname>
      status: success | error
      process-id: <process id of the vm>
    - name: <vm-hostname>
      status: success | error
      process-id: <process id of the vm>
    - ..
  ```
* Erwartetes Verhalten:
  Der für den Knoten zuständige Scheduler empfängt die Nachricht und
  startet die Anwendung in der VM.
* Implementierung:
  Über mqtt_publish in Startup Skript der VM. Hierbei gibt es die
  folgenden Möglichkeiten:
	1. VM sendet an migfra "vm ready", migfra sendet dies gebündelt an
	   scheduler mit "vm started"
	2. Migfra wartet bis alle VMs der task "start vm" per SSH erreichbar
	   sind und schickt gebündelt eine Liste der Stati an den zuständigen
	   scheduler.

#### VM gestoppt
Informiert den zuständigen Scheduler, dass die VM gestoppt ist.
* topic: fast/migfra/<hostname>/status
* Payload
  ```
  result: vm stopped
  list:
    - name: <vm-hostname>
      status: success | error
    - name: <vm-hostname>
      status: success | error
    - ..
  ```
* Erwartetes Verhalten:
  VM aufräumen? Log files für den Nutzer rauskopieren?

#### Migration abgeschlossen
Meldung an den Scheduler dass die Migration fertig ist.
* topic: fast/migfra/<hostname>/status
* Payload
  ```
  result: vm migrated
  name: <vm name>
  status:
    result: success | error
    retries: <counter>
    process-id: <process id of the vm>
  ```
* Erwartetes Verhalten:
  Scheduler markiert ursprüngliche Ressource als frei.

#### Verbindungen abbauen
Meldung an die pscom-Schicht, dass die Verbindungen abgebaut werden sollen.
* topic: fast/pscom/<hostname>/<pid>/request
* Payload
  ```
  task: suspend
  ```
* Erwartetes Verhalten:
  pscom faehr suspend-Protokoll ab

#### Verbindungen aufbauen
Meldung an die pscom-Schicht, dass die Verbindungen wieder hergestellt werden
koennen.
* topic: fast/pscom/<hostname>/<pid>/request
* Payload
  ```
  task: resume
  ```
* Erwartetes Verhalten:
  pscom gibt Verbindungen frei

### Agent
#### Agenten anmelden
Knoten wird gestartet. Meldet und meldet sich beim Scheduler an.
* topic: fast/agent/<hostname>/status
* payload:
  ```
  task: init
  source: <hostname>
  ```
* Erwartetes Verhalten:
  Der für den Knoten zuständige Scheduler empfängt die Nachricht und
  nimmt den Knoten in sein Scheduling mit auf.
* Implementierung:
  Agent wird automatisch bei Systemstart gestartet und verschickt die
  o.g. Nachricht.

#### Messwerte zurückschicken
TODO: In welcher Form will der scheduler die?
### Anwendungen
TODO?
#### Application status
* topic: fast/application/<slurm_jobid>/status
* payload:
  ```
  task: status
  source: <slurm_jobid>
  migration: yes | no
  supported_KPI :  
    - memory usage
    - memory bandwidth
    - ..
  ```
* Erwartetes Verhalten:
  Application is migratable be default.
  However this can be changed by setting 'migration' to 'no' in the status message.
  Also application at start up can declare the list of KPIs that it is welling to provide.
#### Application KPI
* topic: fast/application/<slurm_jobid>/KPIs
* payload:
  ```
  task: KPIs
  source: <slurm_jobid>
  supported_KPI :  
      - memory usage : < value >
      - memory bandwidth : <value>
      - ..
  ```
* Erwartetes Verhalten:
  The scheduler should receive the application's KPIs and take them into account. 
