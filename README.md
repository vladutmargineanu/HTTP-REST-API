# HTTP-REST-API
Homework for the Communication Protocols course @ ACS, UPB 2018

# Algorithm

In arhiva temei am urmatoarele fisiere: client.c, helpers.c, helpers.h si
Makefile. Stim ca REST API are la baza modelul client-server, unde serverul
este implementat ca un black box, scopul temei fiind sa intelegem cum
functioneaza protocolul HTTP.
  In client.c am facut primul request si am creat cate o functie care
sa rezolve fiecare task in parte cerute in enunt.
  Am folosit parserul din libjson-c-dev care mi-a parsat raspunsul pe baza de
tokeni. Am folosit biblioteci externe pentru parsarea si crearea de structuri
JSON. Stim din enunt ca IP-ul serverului este: 185.118.200.35, iar portul este:
8081. 
  Pentru parsare am folosit biblioteca de parsare JSON "#include<json-c/json.h>".
Codul l-am scris sa fie compatibil cu standardul ANSI C pentru parsarea unui 
document JSON simplu. Pentru folosirea bibliotecii, am rulat urmatoarea comanda
pe linux (instalam biblioteca pentru parsare):
[1] sudo apt install libjson-c-dev
  Dupa ce am obtinut raspunsurile venite de la server cu structura clasica si
le-am parsat corespunzator, m-am folosit de functiile compute_get_request si
compute_post_request pentru a-mi creea cererile. Dupa ce am creeat cererile, 
daca o cerere a fost efectuată corect, serverul va intoarce un obiect de tip
JSON, cu mai multe câmpuri, din care obtinem enuntul etapei urmatoare.
  De asemenea am mai folosit functia hostname_to_ip care imi returneaza ip-ul
unei adrese pe care o primeste.
 Pentru a putea folosi datele din răspunsul serverului, am utilizat o bibliotecă
de parsare de JSON pentru extragerea datelor.

 Resurse internet pentru biblioteca de parsare JSON "#include<json-c/json.h>"
am gasit la adresa urmatoare:
[2] https://progur.com/2018/12/how-to-parse-json-in-c.html
