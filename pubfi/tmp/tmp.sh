#!/bin/bash
export PUBLICOM_KEY_ENV='g1jnHfhKWcvmmPdcpzdl9w3lDrZno8yU'
echo PUBLICOM_KEY_ENV $PUBLICOM_KEY_ENV
curl --request POST \
  --url https://api.public.com/userapiauthservice/personal/access-tokens \
  --header 'User-Agent: public-dev-docs' \
  --header 'Content-Type: application/json' \
  --data '{
  "validityInMinutes": 123,
  "secret": "g1jnHfhKWcvmmPdcpzdl9w3lDrZno8yU"
}' 
