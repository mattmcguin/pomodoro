[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo: string = "./logo.svg";

type state = {
  time: int,
  intervalId: option(Js.Global.intervalId),
};

type action =
  | Start(option(Js.Global.intervalId))
  | Clear
  | Increment
  | Stop;

let optionalIntervaltoInterval = intervalId => {};

let component = ReasonReact.reducerComponent("App");

let make = (~message, _children) => {
  ...component,

  initialState: () => {time: 0, intervalId: None},

  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Start(intervalId) => ReasonReact.Update({...state, intervalId})
    | Clear => ReasonReact.Update({...state, intervalId: None, time: 0})
    | Increment => ReasonReact.Update({...state, time: state.time + 1})
    | Stop => ReasonReact.Update({...state, time: 0})
    },

  render: self =>
    <div className="App">
      <div className="App-header">
        <h2> {ReasonReact.string(string_of_int(self.state.time))} </h2>
        <img src=logo className="App-logo" alt="logo" />
        <h2> {ReasonReact.string(message)} </h2>
        <div>
          <button
            onClick={
              _event => {
                let intervalId =
                  Js.Global.setInterval(() => self.send(Increment), 1000);
                self.send(Start(Some(intervalId)));
              }
            }>
            {ReasonReact.string("Start")}
          </button>
          <button
            onClick={
              _event => {
                switch (self.state.intervalId) {
                | Some(intervalId) => Js.Global.clearInterval(intervalId)
                | None => ()
                };
                self.send(Clear);
              }
            }>
            {ReasonReact.string("Stop")}
          </button>
        </div>
      </div>
    </div>,
};