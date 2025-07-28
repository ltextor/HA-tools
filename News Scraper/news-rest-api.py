from fastapi import FastAPI
import uvicorn

app = FastAPI()

@app.get("/api/text")
def get_text():
    return {"message": "Hello from FastAPI!"}

@app.get("/api/text/{name}")
def get_personalized_text(name: str):
    return {"message": f"Hello, {name}!"}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)