const requestBody = {
  "validityInMinutes": 123,
  "secret": "string"
};

fetch("https://api.public.com/userapiauthservice/personal/access-tokens", {
  method: "POST",
  headers: {
    "Content-Type": "application/json",
    "User-Agent": "public-dev-docs"
  },
  body: JSON.stringify(requestBody)
})
.then(response => response.json())
.then(data => console.log(data));
