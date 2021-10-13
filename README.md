Protocole de communication unité PolyBroue

# Protocole basique de communication initial
Le Raspberry Pi envoie une commande en deux octets successifs sur le bus I2C à l'adresse de l'Arduino (`0x20` par défaut) : un ID de device (numéro valve), suivi d'un booléen dictant l'état demandé (`true` => ouvert, `false` => fermé).