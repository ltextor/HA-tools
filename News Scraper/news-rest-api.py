from fastapi import FastAPI
import uvicorn
import requests
from bs4 import BeautifulSoup
import googlemaps
#from datetime import datetime, timedelta
from datetime import datetime
import os
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.cron import CronTrigger
import atexit
from dotenv import load_dotenv
import json
import random

load_dotenv(override=False)                                      # Load .env file, but do not override existing environment variables

rss_feed_url = os.getenv('RSS_URL')                              # set your RSS URL in the environment variable or .env file
article_title = ""
article_date = ""
article_content = ""

fun_facts = []

gmaps = googlemaps.Client(key=os.getenv('GOOGLE_MAPS_API_KEY'))  # set Google Maps API key in the environment variable or .env file
origin_address = os.getenv('ORIGIN_ADDRESS')                     # set your address in the environment variable or .env file

# ---
# scrape website

def get_rss_article_content():
    global article_title, article_date, article_content
    try:
        # get article link from RSS feed
        print(f"Fetching RSS feed from {rss_feed_url}")
        rss_response = requests.get(rss_feed_url, timeout=30)
        rss_response.raise_for_status()
        soup = BeautifulSoup(rss_response.content, "lxml-xml")
        guid = soup.guid
        if not guid:
            # no news > return old news
            print("Currently no news, returning previous news.")
            return {"title": article_title, "date": article_date, "news": article_content}
        article_url = soup.guid.text
        #article_url = soup.find_all('link')[2].text   # get third link from RSS feed
        article_title = soup.find_all('description')[1].text
        article_date = soup.pubDate.text
        print(f"Found article URL: {article_url}")
        # scrape article content
        print(f"Scraping article from {article_url}")
        article_response = requests.get(article_url, timeout=30)
        article_response.raise_for_status()
        # get text content
        soup = BeautifulSoup(article_response.content, "lxml")
        article_elements = soup.find_all(class_='ArticleElement_article-element__q93eL')
        article_content = "\n".join([element.get_text() for element in article_elements])
        return {"title": article_title, "date": article_date, "news": article_content}
    
    except Exception as e:
        error_msg = f"Error: {e}"
        print(error_msg)
        return {"error": error_msg}

# ---
# get random fun fact

def get_fun_fact():
    global fun_facts
    try:
        if not len(fun_facts):
            with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'fun_facts.json'), 'r', encoding='utf-8') as f:
                fun_facts = json.load(f)

        fun_fact = random.choice(fun_facts)
        fun_facts.remove(fun_fact)
        print(f"Selected fun fact: {fun_fact}")
        return json.dumps(fun_fact, ensure_ascii=False, indent=2)
    
    #except FileNotFoundError:
    #    print("File 'fun_facts.json' not found")
    #    return {"error": "File 'fun_facts.json' not found"}
    except Exception as e:
        error_msg = f"Error: {e}"
        print(error_msg)
        return {"error": error_msg}

# ---
# get traffic info

def get_traffic_info(destination):
    try:
        #start = datetime.now() + timedelta(minutes=30)  # 30 minutes from now
        start = datetime.now()  # current time
        directions_result = gmaps.directions(origin_address,
                                        destination,
                                        mode="driving",
                                        language="de",
                                        units="metric",
                                        traffic_model="best_guess",
                                        departure_time=start)
        leg = directions_result[0]['legs'][0]
        route = leg['start_address'] + " nach " + leg['end_address']
        duration = leg['duration']['text']
        duration_in_traffic = leg['duration_in_traffic']['text']
        delay_ratio = leg['duration_in_traffic']['value'] / leg['duration']['value']
        if delay_ratio < 1.1:
            traffic = "Wenig Verkehr"
        elif delay_ratio < 1.3:
            traffic = "Mässig Verkehr"
        elif delay_ratio < 1.5:
            traffic = "Viel Verkehr"
        else:
            traffic = "Sehr viel Verkehr"
        print(f"Route: {route}, Duration: {duration}, Duration in traffic: {duration_in_traffic}, Traffic: {traffic}")
        return {
            "route": route,
            "duration": duration,
            "duration_in_traffic": duration_in_traffic,
            "traffic": traffic
        }
    
    except Exception as e:
        error_msg = f"Error: {e}"
        print(error_msg)
        return {"error": error_msg}
    
# ---
# fetch news when the url is available (url appears around 5 pm and disappears around 11 pm)

def scheduled_news_fetch():
    print(f"Scheduled news fetch at {datetime.now()}")
    get_rss_article_content()

scheduler = BackgroundScheduler()
scheduler.add_job(
    func=scheduled_news_fetch,
    trigger=CronTrigger(hour=17, minute=30, day_of_week='0-4'),   # Monday to Friday 5.30 pm
    id='daily_news_fetch',
    replace_existing=True
)
scheduler.start()
atexit.register(lambda: scheduler.shutdown())
    
# ---
# REST server

app = FastAPI()

@app.get("/api/version")
def get_version():
    return {"version": "v2.1"}

@app.get("/api/dailynews")
def get_dailynews():
    return get_rss_article_content()

@app.get("/api/funfact")
def get_funfact_endpoint():
    return get_fun_fact()

@app.get("/api/trafficinfo/{destination}")
def get_traffic_for_destination(destination: str):
    if not destination.strip():
        return {"error": "Destination cannot be empty"}
    return get_traffic_info(destination)

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)