Build and start (interactive or detached) container with
```
§ sudo docker build -t news_scraper .
§ sudo docker run -it --rm -p 8000:8000 -e GOOGLE_MAPS_API_KEY=XXX --name=news_scraper news_scraper
§ sudo docker run -d --rm -p 8000:8000 -e GOOGLE_MAPS_API_KEY=XXX --name=news_scraper news_scraper
```
