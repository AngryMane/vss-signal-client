# vss-signal-client

TODO:

# recommended environment

* Ubuntu 24.04 lts
* kuksa-data-broker: 0.5.0

# before developing

```
$ source venv/bin/activate
```

# how to setup 

``
`
$ docker run -it --rm --name Server --network kuksa ghcr.io/eclipse-kuksa/kuksa-databroker:0.5.0 --insecure enable-viss &
$ socat TCP-LISTEN:8090,fork TCP:$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' Server):8090 &

```

# how to use

TODO:

