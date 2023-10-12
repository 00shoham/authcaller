# authcaller
Enable two people inside an organization to authenticate one-another via a portal that authenticates both and via sharing a randomly generated, time-limited PIN - for example via a phone call.

BACKGROUND

  A problem that arises in larger organizations is that sometimes people
  within the organization, who have never met, must transact and need to
  confirm one anothers' identities before proceeding.


APPROACH

  Most organizations have authentication systems, including passwords
  (AD, Azure, etc.) and MFA systems (Okta, Duo, etc.).  They generally have
  a mechanism to place web applications "behind" authentication, such that
  users can only interact with web applications after they have signed in.

  This app is intended to be installed behind such enterprise authentication
  systems -- users should not be able to access it without first signing in.
  The app does not perform any authentication.  It depends on being installed
  in such a way that authentication must happen before the app can be reached.

  The app has two modes:

  (a) A user signs in, the app checks their already-established identity
      and the user can generate a PIN.  The PIN is displayed to the user,
      who can share it with others, and the PIN is also stored for a limited
      time.

  (a) A user signs in and enters a PIN provided by another user.  The app
      validates the PIN and displays the identity of the user who generated
      the PIN to the user at the portal.

