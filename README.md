# delta_neutral_market_maker
Delta Neutral Market Maker with Tight Risk Controls
A simple, fast C++ market-making model. 

Uses the Avellaneda–Stoikov model to price quotes

Keeps each position under 2.5% of your portfolio NAV 

Cancels stale orders and throttles fills for safety, with strict risk management 

Hedges instantly in the underlying stock

Is fully configurable via config.yaml

Includes logging, unit tests, and CI setup
