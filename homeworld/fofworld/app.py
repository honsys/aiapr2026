from flask import Flask, jsonify, render_template, request
from worldgen import generate_world

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/world")
def world():
    seed = request.args.get("seed", 42, type=int)
    return jsonify(generate_world(seed))

if __name__ == "__main__":
    app.run(debug=True, port=5050)
