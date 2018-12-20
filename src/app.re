[%bs.raw {|require('./app.css')|}];

[@bs.module] external logo: string = "./logo.svg";
[@bs.module] external audioClip: string = "./alert.mp3";

module JsAudio = {
  type audio;
  [@bs.new] external make: string => audio = "Audio";
  [@bs.send] external play: audio => unit = "play";
};

type state = {
  time: int,
  intervalId: option(Js.Global.intervalId),
  isPaused: bool,
  isOnBreak: bool,
  count: int,
};

type action =
  | Start(option(Js.Global.intervalId))
  | Clear
  | Increment
  | Pause;

let clearInterval = intervalId =>
  switch (intervalId) {
  | Some(intervalId) => Js.Global.clearInterval(intervalId)
  | None => ()
  };

let handleStop = (event, self) => {
  clearInterval(self.ReasonReact.state.intervalId);
  self.send(Clear);
};

let handleStart = (event, self) => {
  clearInterval(self.ReasonReact.state.intervalId);
  let intervalId =
    Js.Global.setInterval(() => self.ReasonReact.send(Increment), 1000);
  self.send(Start(Some(intervalId)));
};

let handlePause = (event, self) => {
  clearInterval(self.ReasonReact.state.intervalId);
  self.send(Pause);
};

let checkTimer = state =>
  state.isOnBreak ?
    if (state.count mod 4 == 0) {
      if (state.time == 1200) {
        JsAudio.(make(audioClip) |> play);
        ReasonReact.Update({...state, time: 0, isOnBreak: false});
      } else {
        ReasonReact.Update({...state, time: state.time + 1});
      };
    } else if (state.time == 300) {
      JsAudio.(make(audioClip) |> play);
      ReasonReact.Update({...state, time: 0, isOnBreak: false});
    } else {
      ReasonReact.Update({...state, time: state.time + 1});
    } :
    (
      if (state.time == 1500) {
        JsAudio.(make(audioClip) |> play);
        ReasonReact.Update({
          ...state,
          time: 0,
          isOnBreak: true,
          count: state.count + 1,
        });
      } else {
        ReasonReact.Update({...state, time: state.time + 1});
      }
    );

let padWithZero = time => String.length(time) == 1 ? "0" ++ time : time;

let component = ReasonReact.reducerComponent("App");

let make = (~message, _children) => {
  ...component,

  /* Initial State */
  initialState: () => {
    time: 0,
    intervalId: None,
    isPaused: true,
    isOnBreak: false,
    count: 0,
  },

  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Start(intervalId) =>
      ReasonReact.Update({...state, intervalId, isPaused: false})
    | Clear =>
      ReasonReact.Update({
        time: 0,
        intervalId: None,
        isPaused: true,
        isOnBreak: false,
        count: 0,
      })
    | Increment => checkTimer(state)
    | Pause => ReasonReact.Update({...state, isPaused: true})
    },

  render: self =>
    <div className="App">
      <div className="App-header">
        <h2>
          {
            ReasonReact.string(
              padWithZero(string_of_int(self.state.time / 60))
              ++ ":"
              ++ padWithZero(string_of_int(self.state.time mod 60)),
            )
          }
        </h2>
        <h4> {ReasonReact.string(string_of_int(self.state.count))} </h4>
        <img src=logo className="App-logo" alt="logo" />
        <h2> {ReasonReact.string(message)} </h2>
        <div className="button-container">
          {
            self.state.isPaused ?
              <button
                className="button start" onClick={self.handle(handleStart)}>
                {ReasonReact.string("Start")}
              </button> :
              <button
                className="button pause" onClick={self.handle(handlePause)}>
                {ReasonReact.string("Pause")}
              </button>
          }
          <button className="button stop" onClick={self.handle(handleStop)}>
            {ReasonReact.string("Stop")}
          </button>
        </div>
      </div>
    </div>,
};