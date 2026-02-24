# Kudos System Specification

## Functional Requirements

### User Stories

1. **As a user, I can select another user from a dropdown list**
   - Users need an easy way to identify colleagues to send kudos to
   - The dropdown should be searchable and filterable
   - Users should not be able to send kudos to themselves

2. **As a user, I can write a message of appreciation (max 500 characters)**
   - Users should have a text input field with character counter
   - The message is optional but recommended
   - Input should support basic formatting (line breaks)

3. **As a user, I can submit the kudos which gets stored in the database**
   - Form validation ensures recipient and message are valid
   - Submission is tracked with timestamp and sender info
   - Confirmation message appears after successful submission
   - User receives feedback on success or error

4. **As a user, I can view a feed of recent kudos on the dashboard**
   - Feed displays kudos in reverse chronological order
   - Each kudos card shows: sender name, recipient name, message, timestamp
   - Feed loads automatically on page load
   - Pagination or infinite scroll for large datasets

5. **As an administrator, I can hide or delete inappropriate kudos messages**
   - Admin interface allows flagging or removing kudos
   - Removed kudos are soft-deleted (retained in database for audit)
   - Audit log tracks who deleted kudos and when
   - Users who had inappropriate kudos removed receive notification

### Acceptance Criteria

**User Story 1:**
- ✓ Dropdown displays all active users except the current user
- ✓ Dropdown is searchable by first name, last name, or email
- ✓ Selected user name appears clearly in the form
- ✓ Selecting a user clears any previous selection error

**User Story 2:**
- ✓ Text input accepts up to 500 characters
- ✓ Character counter displays remaining characters
- ✓ Counter turns red when within 50 characters of limit
- ✓ Newlines are preserved in stored message

**User Story 3:**
- ✓ Form requires a recipient selection before submission
- ✓ Empty messages are rejected with inline error
- ✓ Submission button is disabled while request is processing
- ✓ Success notification appears and form resets after submission
- ✓ Error notification displays for failed submissions with retry option

**User Story 4:**
- ✓ Feed loads with initial 10 most recent kudos
- ✓ Each kudos displays sender, recipient, message, and timestamp
- ✓ Timestamps are formatted as relative time (e.g., "2 hours ago")
- ✓ Feed refreshes every 30 seconds for new kudos
- ✓ Load more button or auto-pagination loads additional kudos

**User Story 5:**
- ✓ Admin panel displays flagged/pending kudos
- ✓ Admin can preview full kudos before removal
- ✓ Removal action requires confirmation
- ✓ Removed kudos disappear from public feed
- ✓ Removal reason can be documented by admin

---

## Edge Cases and Considerations

### Content Moderation

**Spam Prevention:**
- Rate limiting: Users can submit max 5 kudos per day per recipient
- Duplicate detection: Flag identical messages submitted within 1 hour to same recipient
- Account age: New accounts (< 7 days old) can submit max 2 kudos per day
- IP-based detection: Log and flag multiple kudos from same IP address in short time window

**Inappropriate Content:**
- Keyword filtering: Automated screening for common inappropriate words/phrases
- Admin flagging: Any user can flag inappropriate kudos for admin review
- Flagged kudos appear in admin queue with reporter information
- False positive appeals: Users can appeal deletion with explanation to admin

**Duplicate Submissions:**
- Check for identical kudos within 24 hours to same recipient
- Show warning if similarity score > 80% to recent kudos
- Allow user to proceed with duplicate (in case of genuine repetition)
- Log duplicate attempts for spam detection patterns

### Edge Cases to Handle

1. **User Deletion:** When a user is deleted, their sent and received kudos should be anonymized or archived
2. **Sender Leaving Company:** Preserve historical kudos but mark sender as inactive
3. **Self-Kudos Attempt:** Reject with clear message before form submission
4. **Bulk Deletion by Admin:** Track which kudos were deleted together and by whom
5. **Concurrent Modifications:** Handle race conditions if admin deletes while user is viewing
6. **Performance with Large Datasets:** Paginate feed to prevent loading thousands of kudos
7. **Privacy:** Ensure users can only see kudos in shared feed (no private/hidden kudos visible to them)
8. **Notifications:** Throttle notifications to prevent spamming users with multiple deletions

---

## Technical Design

### Database Schema

#### kudos
| Field | Type | Constraints | Description |
|-------|------|-------------|-------------|
| id | UUID | PRIMARY KEY | Unique identifier for kudos |
| sender_id | UUID | FOREIGN KEY (users.id) | ID of user giving kudos |
| recipient_id | UUID | FOREIGN KEY (users.id) | ID of user receiving kudos |
| message | TEXT | MAX 500 chars | Message of appreciation |
| created_at | TIMESTAMP | NOT NULL, DEFAULT NOW() | When kudos was created |
| updated_at | TIMESTAMP | NOT NULL, DEFAULT NOW() | Last update timestamp |
| is_deleted | BOOLEAN | DEFAULT FALSE | Soft delete flag |
| deleted_by_id | UUID | FOREIGN KEY (users.id), NULLABLE | Admin who deleted it |
| deleted_at | TIMESTAMP | NULLABLE | When kudos was deleted |
| deletion_reason | TEXT | NULLABLE | Why kudos was removed |

#### kudos_audit_log
| Field | Type | Constraints | Description |
|-------|------|-------------|-------------|
| id | UUID | PRIMARY KEY | Unique identifier |
| kudos_id | UUID | FOREIGN KEY (kudos.id) | Reference to kudos |
| action | VARCHAR | NOT NULL | 'created', 'deleted', 'flagged' |
| performed_by_id | UUID | FOREIGN KEY (users.id) | Admin or system user |
| action_reason | TEXT | NULLABLE | Reason for action |
| created_at | TIMESTAMP | NOT NULL, DEFAULT NOW() | Action timestamp |

### API Endpoints

#### Submit Kudos
- **Endpoint:** `POST /api/v1/kudos`
- **Authentication:** Required (Bearer token)
- **Request Body:**
  ```json
  {
    "recipient_id": "uuid",
    "message": "string (0-500 chars)"
  }
  ```
- **Response (201 Created):**
  ```json
  {
    "id": "uuid",
    "sender_id": "uuid",
    "recipient_id": "uuid",
    "message": "string",
    "created_at": "ISO 8601 timestamp"
  }
  ```
- **Error Responses:**
  - 400: Invalid payload or message exceeds 500 characters
  - 401: Unauthorized
  - 403: User cannot give kudos to themselves
  - 500: Server error

#### Get Kudos Feed
- **Endpoint:** `GET /api/v1/kudos/feed?limit=10&offset=0`
- **Authentication:** Required (Bearer token)
- **Query Parameters:**
  - `limit`: Number of kudos to return (default: 10, max: 50)
  - `offset`: Number to skip for pagination (default: 0)
- **Response (200 OK):**
  ```json
  {
    "data": [
      {
        "id": "uuid",
        "sender": {"id": "uuid", "name": "string", "email": "string"},
        "recipient": {"id": "uuid", "name": "string", "email": "string"},
        "message": "string",
        "created_at": "ISO 8601 timestamp"
      }
    ],
    "total": 150,
    "limit": 10,
    "offset": 0
  }
  ```

#### Get Users List
- **Endpoint:** `GET /api/v1/users?search=string`
- **Authentication:** Required (Bearer token)
- **Query Parameters:**
  - `search`: Optional search term (first name, last name, or email)
- **Response (200 OK):**
  ```json
  {
    "data": [
      {
        "id": "uuid",
        "name": "string",
        "email": "string",
        "department": "string"
      }
    ]
  }
  ```

#### Delete Kudos (Admin)
- **Endpoint:** `DELETE /api/v1/kudos/{kudos_id}`
- **Authentication:** Required (Bearer token, admin role)
- **Request Body:**
  ```json
  {
    "reason": "string (required, reason for removal)"
  }
  ```
- **Response (204 No Content)**
- **Error Responses:**
  - 403: User is not an admin
  - 404: Kudos not found
  - 500: Server error

### Frontend Components

#### KudosSubmitForm Component
- **Props:** `onSuccess` (callback), `currentUserId` (string)
- **State:** `selectedRecipient`, `message`, `isLoading`, `error`
- **Features:**
  - User dropdown with search functionality
  - Text area with character counter
  - Submit button with loading state
  - Error and success messages
  - Form reset after successful submission

#### KudosFeed Component
- **Props:** `refreshInterval` (milliseconds, default: 30000)
- **State:** `kudos` (array), `isLoading`, `page`, `hasMore`
- **Features:**
  - Displays list of KudosCard components
  - Auto-refresh functionality
  - "Load More" pagination
  - Empty state message when no kudos available
  - Loading skeleton during initial load

#### KudosCard Component
- **Props:** `kudos` (object), `isAdmin` (boolean), `onDelete` (callback)
- **Features:**
  - Displays sender and recipient names
  - Shows kudos message
  - Relative timestamp (e.g., "2 hours ago")
  - Delete button visible only to admins
  - Hover effects for interactivity

#### UserDropdown Component
- **Props:** `value` (uuid), `onChange` (callback), `excludeUserId` (uuid)
- **Features:**
  - Searchable dropdown
  - Debounced API calls for user search
  - Loading state while fetching users
  - Displays user name and email
  - Keyboard navigation support

#### AdminKudosPanel Component
- **Props:** `kudosId` (uuid), `onReviewed` (callback)
- **Features:**
  - Shows flagged/pending kudos
  - Preview of full kudos content
  - Deletion confirmation dialog
  - Reason dropdown for deletion
  - Audit trail display

---

## Implementation Plan

### Phase 1: Backend Setup (Days 1-3)
- [ ] Create database schema (kudos and kudos_audit_log tables)
- [ ] Create database migrations
- [ ] Set up ORM models for Kudos and AuditLog
- [ ] Create Kudos controller with submit endpoint
- [ ] Implement form validation (recipient exists, message length)
- [ ] Add error handling and logging

### Phase 2: API Endpoints (Days 4-5)
- [ ] Implement GET /api/v1/kudos/feed endpoint
- [ ] Implement GET /api/v1/users endpoint with search
- [ ] Implement DELETE /api/v1/kudos/{id} endpoint (admin only)
- [ ] Add middleware for admin role checking
- [ ] Add comprehensive error responses
- [ ] Write API documentation

### Phase 3: Frontend Components (Days 6-9)
- [ ] Build UserDropdown component with search
- [ ] Build KudosSubmitForm component
- [ ] Build KudosCard component
- [ ] Build KudosFeed component with pagination
- [ ] Integrate form submission with API
- [ ] Implement auto-refresh functionality for feed
- [ ] Add form validation and error handling

### Phase 4: Admin Features (Days 10-11)
- [ ] Build AdminKudosPanel component
- [ ] Implement soft delete functionality
- [ ] Add audit logging for all admin actions
- [ ] Create admin notification system
- [ ] Test admin workflows

### Phase 5: Testing & QA (Days 12-14)
- [ ] Unit tests for API endpoints
- [ ] Integration tests for database operations
- [ ] UI component tests
- [ ] End-to-end testing of full workflows
- [ ] Performance testing with large datasets
- [ ] Security review and penetration testing

### Phase 6: Deployment (Day 15)
- [ ] Database migration in production
- [ ] Deploy backend services
- [ ] Deploy frontend application
- [ ] Monitor logs for errors
- [ ] User acceptance testing
- [ ] Documentation and training materials

